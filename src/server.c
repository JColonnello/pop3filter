#include <stdlib.h>
#define _GNU_SOURCE

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
#include "net-utils/tcpUtils.h"
#include "net-utils/udpUtils.h"

#define hasFlag(x, f) (bool)((x) & (f))

static int tcpSockets[2] = {-1};
static int udpSockets[2] = {-1};
static int epollfd;
static bool sigioPending = false;

static Pool *evDataPool;
static Pool *clients;

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
} SocketType;

typedef struct
{
	int fd;
	Handle clientId;
	SocketType type;
	bool readReady, writeReady;
} EventData;

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

	ClientData clientData = {};
	Handle clientId = Pool_Add(clients, &clientData);
	if (clientId < 0)
		log(LOG_FATAL, "Out of memory");

	// Add to epoll
	Handle clientEvent = eventAdd(
	    (EventData){
	        .clientId = clientId,
	        .fd = clientSocket,
	        .type = ST_CLIENT,
	    },
	    EPOLLIN | EPOLLOUT);
	// Set backward reference
	ClientData *ref = Pool_GetRef(clients, clientId);
	ref->clientEvent = clientEvent;
	ref->id = clientId;
	ref->activeSegments = CS_CLIENT_SERVER;

	return false;
}

static void closeClient(ClientData *client, CommSegment level)
{
	switch (level)
	{
	case CS_CLIENT_SERVER: {
		EventData *clientEvent = Pool_GetRef(evDataPool, client->clientEvent);
		clientEvent->readReady = false;
		client->activeSegments &= ~CS_CLIENT_SERVER;
		break;
	}
	case CS_SERVER_CLIENT: {
		EventData *serverEvent = Pool_GetRef(evDataPool, client->serverEvent);
		serverEvent->readReady = false;
		client->activeSegments &= ~CS_SERVER_CLIENT;
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
		EventData *clientEvent = Pool_GetRef(evDataPool, client->clientEvent);
		int fd = clientEvent->fd;
		close(fd);
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
	// EventData *serverEvent = Pool_GetRef(evDataPool, client->serverEvent);

	/// TODO: Process client -> server stream;
	// if (client->activeSegments & CS_CLIENT_SERVER && clientEvent->readReady && serverEvent != NULL &&
	// serverEvent->writeReady)
	if (client->activeSegments & CS_CLIENT_SERVER && clientEvent->readReady)
	{
		bool exhausted;
		// Skip
		static char buffer[CLIENT_WRITE_BUF];
		ssize_t skip = read(clientEvent->fd, buffer, sizeof(buffer));
		if (skip <= 0)
		{
			// Close client
			closeClient(client, CS_CLIENT_SERVER);
			return;
		}
		exhausted = (skip < CLIENT_WRITE_BUF);
		clientEvent->readReady = !exhausted;
	}
	// if (client->activeSegments & CS_SERVER_CLIENT && clientEvent->writeReady && serverEvent != NULL &&
	// serverEvent->readReady)
	if (client->activeSegments & CS_SERVER_CLIENT && clientEvent->writeReady)
	{
		// closeClient(client, CS_SERVER_CLIENT);
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
	if (eventData->type == ST_PASSIVE_UDP && event.events & EPOLLIN)
	{
		struct sockaddr_storage clntAddr; // Client address
		socklen_t clntAddrLen = sizeof(clntAddr);
		static char udpBuffer[32];
		ssize_t numBytesRcvd =
		    recvfrom(eventData->fd, udpBuffer, sizeof(udpBuffer) - 1, 0, (struct sockaddr *)&clntAddr, &clntAddrLen);
		udpBuffer[numBytesRcvd] = 0;
		processCmd(udpBuffer, numBytesRcvd, eventData->fd, (struct sockaddr *)&clntAddr, clntAddrLen, serverArguments);
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

static void sigioHandler()
{
	sigioPending = true;
}

static void closeServer()
{
	close(epollfd);
	for (int i = 0; i < sizeof(tcpSockets) / sizeof(*tcpSockets); i++)
		if (tcpSockets[i] > 0)
			close(tcpSockets[i]);
	for (int i = 0; i < sizeof(udpSockets) / sizeof(*udpSockets); i++)
		if (udpSockets[i] > 0)
			close(udpSockets[i]);

	Pool_Dispose(evDataPool);
	Pool_Dispose(clients);
	exit(0);
}

void startServer(const char *port)
{
	// Close stdio
	close(STDIN_FILENO);
	open("/dev/null", O_WRONLY);

	// Set signal handler for SIGIO
	struct sigaction handler = (struct sigaction){.sa_handler = sigioHandler};
	sigfillset(&handler.sa_mask);
	if (sigaction(SIGIO, &handler, NULL) < 0)
		log(LOG_FATAL, "sigaction() failed for SIGIO");

	handler.sa_handler = closeServer;
	sigaction(SIGINT, &handler, NULL);

	// Enable SIGIO
	sigset_t mask, oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGIO);
	sigprocmask(SIG_UNBLOCK, &mask, &oldmask);

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
		sigset_t oldMask;
		sigprocmask(SIG_BLOCK, &sigioMask, &oldMask);
		if (sigioPending)
		{
			/// TODO: Process signal
			sigioPending = false;
		}

		// Wait for events
		int nfds = epoll_pwait(epollfd, events, max_events, -1, &oldMask);
		if (nfds == -1)
		{
			if (errno == EINTR)
				nfds = 0;
			else
				log(LOG_FATAL, "epoll_pwait");
		}

		// Re-enable SIGIO
		sigprocmask(SIG_UNBLOCK, &sigioMask, NULL);

		for (int i = 0; i < nfds; i++)
			processEvent(events[i]);

		int clientCount = Pool_Count(clients);
		Handle activeClients[clientCount];
		Pool_ToIndexArray(clients, activeClients);
		for (int i = 0; i < clientCount; i++)
			processClient(Pool_GetRef(clients, activeClients[i]));
	}
}