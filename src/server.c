#include "collections/queue.h"
#include <stdbool.h>
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
#include <sys/ioctl.h>
#include <unistd.h>

#include "logger.h"
#include "management/management.h"
#include "net-utils/connect.h"
#include "net-utils/tcpUtils.h"
#include "net-utils/udpUtils.h"
#include "server.h"

#define hasFlag(x, f) (bool)((x) & (f))

#define PENDING_QUEUE 10

static int tcpSockets[2] = {-1};
static int udpSockets[2] = {-1};
static int epollfd;
static Pool *evDataPool;
static Pool *clients;
static volatile sig_atomic_t exitProgram = 0;

ServerArguments serverArguments;

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
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, eventData.fdrw, &ev) == -1)
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
	    .filterEvent = -1,
	};
	Handle clientId = Pool_Add(clients, &clientData);
	if (clientId < 0)
		log(LOG_FATAL, "Out of memory");
	// Set backward reference
	ClientData *ref = Pool_GetRef(clients, clientId);
	ref->id = clientId;

	// Reserve parser buffers
	ref->responseState = (Input){0};
	ref->requestState = (Input){0};
	ref->responseState.bufSize = 512;
	ref->responseState.buf = malloc(ref->responseState.bufSize + 1);
	ref->responseState.writeBuf = malloc(ref->responseState.bufSize * 2 + 2);
	ref->requestState.bufSize = 256;
	ref->requestState.buf = malloc(ref->requestState.bufSize + 1);
	ref->requestState.writeBuf = malloc(ref->requestState.bufSize * 2 + 2);
	initState(&ref->responseState);
	initState(&ref->requestState);

	ref->commandQueue = Queue_Create(PENDING_QUEUE, sizeof(PendingRequest));
	PendingRequest req = {.cmd = POP_INCOMPLETE};
	ref->pending = true;
	Queue_Enqueue(ref->commandQueue, &req);

	// Add to epoll
	Handle clientEvent = eventAdd(
	    (EventData){
	        .clientId = clientId,
	        .fdrw = clientSocket,
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
	        .fdrw = serverSocket,
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
		shutdown(clientEvent->fdrw, SHUT_RD);
		shutdown(serverEvent->fdrw, SHUT_WR);
		break;
	}
	case CS_SERVER_CLIENT: {
		closeClient(client, CS_CLIENT_SERVER);
		serverEvent->readReady = false;
		serverEvent->writeReady = false;
		client->activeSegments &= ~CS_SERVER_CLIENT;
		shutdown(serverEvent->fdrw, SHUT_RD);
		shutdown(clientEvent->fdrw, SHUT_WR);
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
		if (clientEvent->fdrw > 0)
			close(clientEvent->fdrw);
		if (serverEvent->fdrw > 0)
			close(serverEvent->fdrw);
		// epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
	}

	// Free client structures
	if (!client->activeSegments)
	{
		free(client->requestState.buf);
		free(client->responseState.buf);
		free(client->requestState.writeBuf);
		free(client->responseState.writeBuf);
		while (Queue_Count(client->commandQueue) > 0)
		{
			PendingRequest req;
			Queue_Dequeue(client->commandQueue, &req);
			free(req.data);
		}
		Queue_Dispose(client->commandQueue);
		Pool_Remove(evDataPool, client->clientEvent);
		Pool_Remove(evDataPool, client->serverEvent);
		Pool_Remove(clients, client->id);
	}
}

static bool processClient(ClientData *client)
{
	bool advanced = false;
	EventData *clientEvent = Pool_GetRef(evDataPool, client->clientEvent);
	EventData *serverEvent = Pool_GetRef(evDataPool, client->serverEvent);
	EventData *filterEvent = Pool_GetRef(evDataPool, client->filterEvent);

	// Waiting for connection
	if (client->serverEvent >= 0 && !(client->activeSegments & CS_SERVER_CLIENT) && serverEvent->writeReady)
	{
		advanced = true;
		int error;
		uint size = sizeof(error);
		getsockopt(serverEvent->fdrw, SOL_SOCKET, SO_ERROR, &error, &size);
		if (error != 0)
		{
			closeClient(client, CS_CLIENT_SERVER);
		}
		else
			client->activeSegments |= CS_SERVER_CLIENT;
	}
	// Process server -> filter stream;
	if (client->activeSegments & CS_SERVER_FILTER && filterEvent->writeReady && serverEvent->readReady)
	{
		advanced = true;
		int copied;
		int completed = copyMsg(&client->responseState, serverEvent->fdrw, filterEvent->fdw, &copied, false);
		int bytes;
		serverEvent->readReady = (ioctl(serverEvent->fdrw, FIONREAD, &bytes), bytes > 0);
		filterEvent->writeReady = (ioctl(filterEvent->fdw, TIOCOUTQ, &bytes), bytes < CLIENT_WRITE_BUF * 2);

		if (completed)
		{
			closeClient(client, CS_SERVER_FILTER);
			return true;
		}
	}
	// Process filter -> server stream;
	if (client->activeSegments & CS_FILTER_CLIENT && filterEvent->readReady && clientEvent->writeReady)
	{
		advanced = true;
		int copied;
		int completed = copyMsg(&client->responseState, filterEvent->fdrw, clientEvent->fdrw, &copied, true);
		int bytes;
		filterEvent->readReady = (ioctl(filterEvent->fdw, FIONREAD, &bytes), bytes > 0);
		clientEvent->writeReady = (ioctl(clientEvent->fdrw, TIOCOUTQ, &bytes), bytes < CLIENT_WRITE_BUF * 2);

		if (completed)
		{
			closeClient(client, CS_FILTER_CLIENT);
			return true;
		}
	}
	// Process client commands
	if (client->activeSegments & CS_CLIENT_SERVER && clientEvent->readReady &&
	    Queue_Count(client->commandQueue) < PENDING_QUEUE)
	{
		advanced = true;
		ssize_t skip = processPopClient(&client->requestState, clientEvent->fdrw, client->commandQueue);
		if (skip <= 0)
		{
			closeClient(client, CS_CLIENT_SERVER);
		}
		int bytes;
		clientEvent->readReady = (ioctl(clientEvent->fdrw, FIONREAD, &bytes), bytes > 0);
	}
	// Read server responses
	if (client->activeSegments & CS_SERVER_CLIENT && clientEvent->writeReady && serverEvent->readReady)
	{
		advanced = true;
		bool redirect = false;
		ssize_t count = processPopServer(client, serverEvent->fdrw, clientEvent->fdrw, &redirect);
		if (count <= 0)
		{
			closeClient(client, CS_SERVER_CLIENT);
			return true;
		}
		int bytes;
		serverEvent->readReady = (ioctl(serverEvent->fdrw, FIONREAD, &bytes), bytes > 0);
	}
	// Send commands to server
	if (client->activeSegments & CS_CLIENT_SERVER && !client->pending && serverEvent->writeReady &&
	    Queue_Count(client->commandQueue) > 0)
	{
		advanced = true;
		PendingRequest req;
		Queue_Peek(client->commandQueue, &req);
		ssize_t skip = sendPopRequest(serverEvent->fdrw, req);
		if (skip <= 0)
		{
			closeClient(client, CS_CLIENT_SERVER);
			return true;
		}
		client->pending = true;
		int bytes;
		clientEvent->writeReady = (ioctl(clientEvent->fdrw, TIOCOUTQ, &bytes), bytes < CLIENT_WRITE_BUF * 2);
	}
	if (!(client->activeSegments & CS_CLIENT_SERVER) && Queue_Count(client->commandQueue) == 0)
	{
		closeClient(client, CS_SERVER_CLIENT);
		return true;
	}
	return advanced;
}

static void processEvent(struct epoll_event event)
{
	EventData *eventData = Pool_GetRef(evDataPool, event.data.u32);
	if (eventData->type == ST_PASSIVE_TCP && event.events & EPOLLIN)
	{
		eventData->readReady = true;
	}
	else if (eventData->type == ST_PASSIVE_UDP && event.events & EPOLLIN)
	{
		struct sockaddr_storage clntAddr; // Client address
		socklen_t clntAddrLen = sizeof(clntAddr);
		static char udpBuffer[32];
		ssize_t numBytesRcvd =
		    recvfrom(eventData->fdrw, udpBuffer, sizeof(udpBuffer) - 1, 0, (struct sockaddr *)&clntAddr, &clntAddrLen);
		udpBuffer[numBytesRcvd] = 0;
		processCmd(udpBuffer, numBytesRcvd, eventData->fdrw, (struct sockaddr *)&clntAddr, clntAddrLen,
		           serverArguments);
	}
	else if (eventData->type == ST_SIGNAL && event.events & EPOLLIN)
	{
		/// TODO: signalfd manage
		signalfd_read(eventData->fdrw);
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
		        .fdrw = tcpSockets[i],
		        .type = ST_PASSIVE_TCP,
		    },
		    EPOLLIN);

	count = setupUDPServerSocket("4321", udpSockets);
	if (count <= 0)
		log(LOG_FATAL, "Cannot open UDP socket");
	for (int i = 0; i < count; i++)
		eventAdd(
		    (EventData){
		        .fdrw = udpSockets[i],
		        .type = ST_PASSIVE_UDP,
		    },
		    EPOLLIN);

	int sigfd = signalfd_setup();
	// Add signalfd to epoll
	eventAdd(
	    (EventData){
	        .fdrw = sigfd,
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
	const int max_events = 10;
	struct epoll_event events[max_events];
	int nfds;
	bool advanced = false;
	for (;;)
	{
		nfds = epoll_wait(epollfd, events, max_events, advanced ? 0 : -1);
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

		advanced = false;
		for (int i = 0; i < sizeof(tcpSockets) / sizeof(*tcpSockets); i++)
			if (tcpSockets[i] > 0)
				advanced |= !acceptClient(tcpSockets[i]);

		int clientCount = Pool_Count(clients);
		Handle activeClients[clientCount];
		Pool_ToIndexArray(clients, activeClients);
		for (int i = 0; i < clientCount; i++)
			advanced |= processClient(Pool_GetRef(clients, activeClients[i]));
	}
}