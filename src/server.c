#define _GNU_SOURCE
#include <stdlib.h>
#include <sys/socket.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "collections/pool.h"
#include "collections/queue.h"
#include "logger.h"
#include "management/management.h"
#include "net-utils/connect.h"
#include "net-utils/tcpUtils.h"
#include "net-utils/udpUtils.h"

#define hasFlag(x, f) (bool)((x) & (f))

static int tcpSockets[2] = {-1};
static int udpSockets[2] = {-1};
static int epollfd;
static Pool *evDataPool;
static Pool *clients;
static volatile sig_atomic_t exitProgram = 0;

ServerArguments serverArguments;

typedef enum
{
	CS_CLIENT_SERVER = 1,
	CS_SERVER_CLIENT = 2,
	CS_SERVER_FILTER = 4,
	CS_FILTER_CLIENT = 8,
} CommSegment;

typedef struct
{
	int id;
	Handle clientEvent;
	Handle serverEvent;
	CommSegment activeSegments;
} ClientData;

typedef enum
{
	ST_PASSIVE_TCP,
	ST_PASSIVE_UDP,
	ST_CLIENT,
	ST_SERVER,
	ST_SIGNAL,
} SocketType;

typedef struct
{
	int fd;
	Handle clientId;
	SocketType type;
	bool readReady, writeReady;
} EventData;

static void closeClient(ClientData *client, CommSegment level);

static Handle eventAdd(EventData eventData, uint32_t epollFlags)
{
	Handle eventId = Pool_Add(evDataPool, &eventData);
	if (eventId < 0)
		log(LOG_FATAL, "Out of memory");

	struct epoll_event ev = (struct epoll_event){
	    .events = epollFlags | EPOLLET,
	    .data.u32 = eventId,
	};
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, eventData.fd, &ev) == -1)
		log(LOG_FATAL, "epoll_ctl: conn_sock");

	return eventId;
}

static bool acceptClient(int socket)
{
	int clientSocket = acceptTCPConnection(socket);
	// No pending connection
	if (clientSocket < 0)
		return true;

	ClientData clientData = {
	    .serverEvent = -1,
	    .clientEvent = -1,
	};
	Handle clientId = Pool_Add(clients, &clientData);
	if (clientId < 0)
		log(LOG_FATAL, "Out of memory");
	// Set backward reference
	ClientData *ref = Pool_GetRef(clients, clientId);
	ref->id = clientId;

	// Add to epoll
	Handle clientEvent = eventAdd(
	    (EventData){
	        .clientId = clientId,
	        .fd = clientSocket,
	        .type = ST_CLIENT,
	    },
	    EPOLLIN | EPOLLOUT);
	ref->clientEvent = clientEvent;
	ref->activeSegments = CS_CLIENT_SERVER;

	// Start connection to server
	int serverSocket = connectHost();
	if (serverSocket < 0)
	{
		closeClient(ref, CS_CLIENT_SERVER);
		return false;
	}
	Handle serverEvent = eventAdd(
	    (EventData){
	        .clientId = clientId,
	        .fd = serverSocket,
	        .type = ST_SERVER,
	    },
	    EPOLLIN | EPOLLOUT);
	ref->serverEvent = serverEvent;

	return false;
}

static void closeClient(ClientData *client, CommSegment level)
{
	EventData *clientEvent = Pool_GetRef(evDataPool, client->clientEvent);
	EventData *serverEvent = Pool_GetRef(evDataPool, client->serverEvent);
	switch (level)
	{
	case CS_CLIENT_SERVER: {
		clientEvent->readReady = false;
		client->activeSegments &= ~CS_CLIENT_SERVER;
		// shutdown(clientEvent->fd, SHUT_RD);
		// if (serverEvent != NULL)
		// shutdown(serverEvent->fd, SHUT_WR);
		break;
	}
	case CS_SERVER_CLIENT: {
		closeClient(client, CS_CLIENT_SERVER);
		serverEvent->readReady = false;
		serverEvent->writeReady = false;
		client->activeSegments &= ~CS_SERVER_CLIENT;
		// shutdown(serverEvent->fd, SHUT_RD);
		// if (clientEvent != NULL)
		// shutdown(clientEvent->fd, SHUT_WR);
		break;
	}
	default:
		break;
	}

	// Client-server is closed both ways
	if ((CS_CLIENT_SERVER | CS_SERVER_CLIENT) & level && !hasFlag(client->activeSegments, CS_SERVER_CLIENT) &&
	    !hasFlag(client->activeSegments, CS_CLIENT_SERVER))
	{
		// Don't release EventData, handler could still be referenced
		if (clientEvent->fd > 0)
			close(clientEvent->fd);
		if (serverEvent->fd > 0)
			close(serverEvent->fd);
		// epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
	}

	// Free client structures
	if (!client->activeSegments)
	{
		Pool_Remove(evDataPool, client->clientEvent);
		Pool_Remove(evDataPool, client->serverEvent);
		Pool_Remove(clients, client->id);
	}
}

static void processClient(ClientData *client)
{
	EventData *clientEvent = Pool_GetRef(evDataPool, client->clientEvent);
	EventData *serverEvent = Pool_GetRef(evDataPool, client->serverEvent);

	// Waiting for connection
	if (client->serverEvent >= 0 && !(client->activeSegments & CS_SERVER_CLIENT) && serverEvent->writeReady)
	{
		int error;
		uint size = sizeof(error);
		getsockopt(serverEvent->fd, SOL_SOCKET, SO_ERROR, &error, &size);
		if (error != 0)
		{
			closeClient(client, CS_CLIENT_SERVER);
			return;
		}
		else
			client->activeSegments |= CS_SERVER_CLIENT;
	}
	/// TODO: Process client -> server stream;
	if (client->activeSegments & CS_CLIENT_SERVER && clientEvent->readReady && serverEvent->writeReady)
	{
		bool exhausted;
		// Skip
		static char buffer[CLIENT_WRITE_BUF];
		ssize_t skip = read(clientEvent->fd, buffer, sizeof(buffer));
		if (skip <= 0)
		{
			// Close client
			closeClient(client, skip < 0 ? CS_SERVER_CLIENT : CS_CLIENT_SERVER);
			return;
		}
		exhausted = (skip < CLIENT_WRITE_BUF);
		clientEvent->readReady = !exhausted;
		int sent = write(serverEvent->fd, buffer, skip);
	}
	if (client->activeSegments & CS_SERVER_CLIENT && clientEvent->writeReady && serverEvent->readReady)
	{
		bool exhausted;
		// Skip
		static char buffer[CLIENT_WRITE_BUF];
		ssize_t skip = read(serverEvent->fd, buffer, sizeof(buffer));
		if (skip <= 0)
		{
			// Close client
			closeClient(client, CS_SERVER_CLIENT);
			return;
		}
		exhausted = (skip < CLIENT_WRITE_BUF);
		serverEvent->readReady = !exhausted;
		int sent = write(clientEvent->fd, buffer, skip);
	}
}

static void processEvent(struct epoll_event event)
{
	EventData *eventData = Pool_GetRef(evDataPool, event.data.u32);
	if (eventData->type == ST_PASSIVE_TCP && event.events & EPOLLIN)
	{
		bool newClient = acceptClient(eventData->fd);
		// Could have more clients waiting, reschedule
		eventData->readReady = newClient;
	}
	else if (eventData->type == ST_PASSIVE_UDP && event.events & EPOLLIN)
	{
		struct sockaddr_storage clntAddr; // Client address
		socklen_t clntAddrLen = sizeof(clntAddr);
		static char udpBuffer[32];
		ssize_t numBytesRcvd =
		    recvfrom(eventData->fd, udpBuffer, sizeof(udpBuffer) - 1, 0, (struct sockaddr *)&clntAddr, &clntAddrLen);
		udpBuffer[numBytesRcvd] = 0;
		processCmd(udpBuffer, numBytesRcvd, eventData->fd, (struct sockaddr *)&clntAddr, clntAddrLen, serverArguments);
	}
	else if (eventData->type == ST_SIGNAL && event.events & EPOLLIN)
	{
		/// TODO: signalfd manage
		signalfd_read(eventData->fd);
	}
	else if (eventData->type)
	{
		if (event.events & EPOLLOUT)
			eventData->writeReady = true;
		if (event.events & EPOLLIN)
			eventData->readReady = true;
	}
	else
		log(LOG_ERROR, "Unhandled event");
}

static void closeServer()
{
	exitProgram = true;
}

void startServer(const char *port)
{
	// Close stdio
	close(STDIN_FILENO);

	// Set signal handler for SIGIO
	struct sigaction handler = {0};
	// handler.sa_handler = sigioHandler;
	sigfillset(&handler.sa_mask);
	// if (sigaction(SIGIO, &handler, NULL) < 0)
	// 	log(LOG_FATAL, "sigaction() failed for SIGIO");

	handler.sa_handler = closeServer;
	if (sigaction(SIGINT, &handler, NULL) < 0)
		log(LOG_FATAL, "sigaction() failed for SIGINT");
	sigaction(SIGPIPE, &(struct sigaction){.sa_handler = SIG_IGN}, NULL);

	// Enable SIGIO
	// sigset_t mask, oldmask;
	// sigemptyset(&mask);
	// sigaddset(&mask, SIGIO);
	// sigprocmask(SIG_UNBLOCK, &mask, &oldmask);

	// Create epoll fd
	epollfd = epoll_create1(0);
	if (epollfd == -1)
		log(LOG_FATAL, "Failed creating epoll handler");

	// Create collections
	evDataPool = Pool_Create(sizeof(EventData));
	clients = Pool_Create(sizeof(ClientData));

	// Add passive socket to epoll

	int count = setupTCPServerSocket(port, tcpSockets);
	if (count <= 0)
		log(LOG_FATAL, "Cannot open TCP socket");
	for (int i = 0; i < count; i++)
		eventAdd(
		    (EventData){
		        .fd = tcpSockets[i],
		        .type = ST_PASSIVE_TCP,
		    },
		    EPOLLIN);

	count = setupUDPServerSocket("4321", udpSockets);
	if (count <= 0)
		log(LOG_FATAL, "Cannot open UDP socket");
	for (int i = 0; i < count; i++)
		eventAdd(
		    (EventData){
		        .fd = udpSockets[i],
		        .type = ST_PASSIVE_UDP,
		    },
		    EPOLLIN);

	int sigfd = signalfd_setup();
	// Add signalfd to epoll
	eventAdd(
	    (EventData){
	        .fd = sigfd,
	        .type = ST_SIGNAL,
	    },
	    EPOLLIN);

	if (resolve_dns("localhost", "2110", 0))
		log(LOG_FATAL, "Could not resolve hostname");
}

void processingLoop()
{
	sigset_t sigioMask;
	sigemptyset(&sigioMask);
	sigaddset(&sigioMask, SI_SIGIO);
	for (;;)
	{
		const int max_events = 10;
		struct epoll_event events[max_events];

		// Block and check for SIGIO event
		// sigset_t oldMask;
		// sigprocmask(SIG_BLOCK, &sigioMask, &oldMask);
		// if (sigioPending)
		// {
		// 	/// TODO: Process signal
		// 	sigioPending = false;
		// }

		// Wait for events
		int nfds = epoll_wait(epollfd, events, max_events, -1);
		if (nfds == -1)
		{
			if (errno == EINTR)
				nfds = 0;
			else
				log(LOG_FATAL, "epoll_pwait");
		}
		if (exitProgram)
		{
			log(LOG_INFO, "Closing...");
			close(epollfd);
			for (int i = 0; i < sizeof(tcpSockets) / sizeof(*tcpSockets); i++)
				if (tcpSockets[i] > 0)
					close(tcpSockets[i]);
			for (int i = 0; i < sizeof(udpSockets) / sizeof(*udpSockets); i++)
				if (udpSockets[i] > 0)
					close(udpSockets[i]);

			free_dns();

			Pool_Dispose(evDataPool);
			Pool_Dispose(clients);
			exit(0);
		}

		// Re-enable SIGIO
		// sigprocmask(SIG_UNBLOCK, &sigioMask, NULL);

		for (int i = 0; i < nfds; i++)
			processEvent(events[i]);

		int clientCount = Pool_Count(clients);
		Handle activeClients[clientCount];
		Pool_ToIndexArray(clients, activeClients);
		for (int i = 0; i < clientCount; i++)
			processClient(Pool_GetRef(clients, activeClients[i]));
	}
}