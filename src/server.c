#define _GNU_SOURCE
#include "collections/pool.h"
#include "collections/queue.h"
#include "pop3/pop3.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "collections/pool.h"
#include "collections/queue.h"
#include "functions.h"
#include "logger.h"
#include "management/management.h"
#include "net-utils/connect.h"
#include "net-utils/tcpUtils.h"
#include "net-utils/udpUtils.h"
#include "server.h"

#define hasFlag(x, f) (bool)((x) & (f))
#define checkEOF(count) (count == 0 || (count < 0 && errno != EAGAIN))

#define PENDING_QUEUE 10

static Handle tcpEvents[2] = {-1};
static Handle udpEvents[2] = {-1};
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

	if (eventData.fdw == 0)
	{
		struct epoll_event ev = (struct epoll_event){
		    .events = epollFlags | EPOLLET,
		    .data.u32 = eventId,
		};
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, eventData.fdrw, &ev) == -1)
			log(LOG_FATAL, "epoll_ctl: conn_sock");
	}
	else
	{
		struct epoll_event ev = (struct epoll_event){
		    .events = EPOLLIN | EPOLLET,
		    .data.u32 = eventId,
		};
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, eventData.fdrw, &ev) == -1)
			log(LOG_FATAL, "epoll_ctl: conn_sock");

		ev = (struct epoll_event){
		    .events = EPOLLOUT | EPOLLET,
		    .data.u32 = eventId,
		};
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, eventData.fdw, &ev) == -1)
			log(LOG_FATAL, "epoll_ctl: conn_sock");
	}

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
	ref->filterState.bufSize = 512;
	ref->filterState.buf = malloc(ref->filterState.bufSize + 1);
	ref->filterState.writeBuf = malloc(ref->filterState.bufSize * 2 + 2);
	ref->requestState.bufSize = 256;
	ref->requestState.buf = malloc(ref->requestState.bufSize + 1);
	ref->requestState.writeBuf = malloc(ref->requestState.bufSize * 2 + 2);
	initState(&ref->responseState);
	initState(&ref->filterState);
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
	ref->activeSegments = CS_CLIENT;

	// Start connection to server
	int serverSocket = connectHost();
	if (serverSocket < 0)
	{
		closeClient(ref, CS_CLIENT);
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
	EventData *filterEvent = Pool_GetRef(evDataPool, client->filterEvent);

	level &= client->activeSegments;
	if (hasFlag(level, CS_CLIENT_OUT))
	{
		clientEvent->readReady = false;
		shutdown(clientEvent->fdrw, SHUT_RD);
	}
	if (hasFlag(level, CS_SERVER_IN))
	{
		serverEvent->writeReady = false;
		shutdown(serverEvent->fdrw, SHUT_WR);
	}
	if (hasFlag(level, CS_CLIENT_IN))
	{
		clientEvent->writeReady = false;
		shutdown(clientEvent->fdrw, SHUT_WR);
	}
	if (hasFlag(level, CS_SERVER_OUT))
	{
		serverEvent->readReady = false;
		shutdown(serverEvent->fdrw, SHUT_RD);
	}
	if (hasFlag(level, CS_FILTER_IN))
	{
		close(filterEvent->fdw);
		filterEvent->writeReady = false;
	}
	if (hasFlag(level, CS_FILTER_OUT))
	{
		close(filterEvent->fdrw);
		filterEvent->readReady = false;
	}
	client->activeSegments &= ~level;

	// Client-server is closed both ways
	if (hasFlag(level, CS_CLIENT) && !hasFlag(client->activeSegments, CS_CLIENT))
	{
		// Don't release EventData, handler could still be referenced
		if (clientEvent->fdrw > 0)
			close(clientEvent->fdrw);
	}
	if (hasFlag(level, CS_SERVER) && !hasFlag(client->activeSegments, CS_SERVER))
	{
		if (serverEvent->fdrw > 0)
			close(serverEvent->fdrw);
	}
	// Filter is closed both sides
	if (hasFlag(level, CS_FILTER) && !hasFlag(client->activeSegments, CS_FILTER))
	{
		Pool_Remove(evDataPool, client->filterEvent);
		client->filterEvent = -1;
	}

	if (hasFlag(level, CS_CLIENT_IN))
	{
		closeClient(client, CS_CLIENT_OUT | CS_SERVER | CS_FILTER);
		return;
	}

	if (hasFlag(level, CS_SERVER_OUT))
	{
		closeClient(client, CS_SERVER_IN);
		return;
	}

	// Free client structures
	if (!client->activeSegments)
	{
		free(client->requestState.buf);
		free(client->responseState.buf);
		free(client->filterState.buf);
		free(client->requestState.writeBuf);
		free(client->responseState.writeBuf);
		free(client->filterState.writeBuf);
		if (client->user)
			free(client->user);
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
	if (client->serverEvent >= 0 && !hasFlag(client->activeSegments, CS_SERVER) && serverEvent->writeReady)
	{
		advanced = true;
		int error;
		uint size = sizeof(error);
		getsockopt(serverEvent->fdrw, SOL_SOCKET, SO_ERROR, &error, &size);
		if (error != 0)
		{
			closeClient(client, CS_CLIENT);
			return true;
		}
		else
			client->activeSegments |= CS_SERVER;
	}
	// Process server -> filter stream;
	if (hasFlag(client->activeSegments, CS_SERVER_OUT) && hasFlag(client->activeSegments, CS_FILTER_IN))
	{
		if (filterEvent->writeReady && serverEvent->readReady)
		{
			advanced = true;
			fillBuffer(&client->responseState, serverEvent->fdrw);
			bool completed = copyMsg(&client->responseState, filterEvent->fdw, false);
			int bytes;
			serverEvent->readReady = (ioctl(serverEvent->fdrw, FIONREAD, &bytes), bytes > 0);
			filterEvent->writeReady = (ioctl(filterEvent->fdw, TIOCOUTQ, &bytes), bytes < CLIENT_WRITE_BUF * 2);

			if (completed)
			{
				closeClient(client, CS_FILTER_IN);
				return true;
			}
		}
	}
	// Read server responses
	else if (hasFlag(client->activeSegments, CS_SERVER_OUT) && hasFlag(client->activeSegments, CS_CLIENT_IN) &&
	         clientEvent->writeReady && serverEvent->readReady && Queue_Count(client->commandQueue) > 0)
	{
		advanced = true;
		bool redirect = false;
		ssize_t count = fillBuffer(&client->responseState, serverEvent->fdrw);
		int completed = processPopServer(client, clientEvent->fdrw, &redirect);
		if (completed < 0)
		{
			closeClient(client, CS_CLIENT | CS_SERVER);
			return true;
		}
		if (checkEOF(count) && !completed)
		{
			closeClient(client, CS_SERVER_OUT);
			return true;
		}

		if (redirect && completed)
		{
			int fd[2];
			pipe2(fd, O_NONBLOCK);
			Handle handle = eventAdd(
			    (EventData){
			        .clientId = client->id,
			        .fdrw = fd[0],
			        .fdw = fd[1],
			        .type = ST_FILTER,
			    },
			    EPOLLIN | EPOLLOUT);
			client->filterEvent = handle;
			filterEvent = Pool_GetRef(evDataPool, handle);
			client->activeSegments |= CS_FILTER;
		}
		else
		{
			serverEvent->readReady = count > 0 && completed;
		}
	}
	// Process filter -> client stream;
	if (hasFlag(client->activeSegments, CS_FILTER_OUT) && hasFlag(client->activeSegments, CS_CLIENT_IN) &&
	    filterEvent->readReady && clientEvent->writeReady)
	{
		advanced = true;
		ssize_t copied = fillBuffer(&client->filterState, filterEvent->fdrw);
		bool completed = copyMsg(&client->filterState, clientEvent->fdrw, true);
		int bytes;
		filterEvent->readReady = copied > 0;
		clientEvent->writeReady = (ioctl(clientEvent->fdrw, TIOCOUTQ, &bytes), bytes < CLIENT_WRITE_BUF * 2);

		if (completed)
		{
			closeClient(client, CS_FILTER_OUT);
			return true;
		}
	}
	// Process client commands
	if (hasFlag(client->activeSegments, CS_CLIENT_OUT) && hasFlag(client->activeSegments, CS_SERVER_IN) &&
	    clientEvent->readReady && Queue_Count(client->commandQueue) < PENDING_QUEUE)
	{
		advanced = true;
		ssize_t count = fillBuffer(&client->requestState, clientEvent->fdrw);
		bool complete = processPopClient(&client->requestState, clientEvent->fdrw, client->commandQueue, &client->user);
		if (checkEOF(count) && !complete)
		{
			closeClient(client, CS_CLIENT_OUT);
			return true;
		}
		clientEvent->readReady = complete;
	}

	// Send commands to server
	if (hasFlag(client->activeSegments, CS_SERVER_IN) && !client->pending && serverEvent->writeReady &&
	    Queue_Count(client->commandQueue) > 0)
	{
		advanced = true;
		PendingRequest req;
		Queue_Peek(client->commandQueue, &req);
		if (req.cmd != POP_INVALID)
		{
			ssize_t count = sendPopRequest(serverEvent->fdrw, req);
			if (checkEOF(count))
			{
				closeClient(client, CS_SERVER_IN);
				return true;
			}
			client->pending = true;
			int bytes;
			clientEvent->writeReady = (ioctl(clientEvent->fdrw, TIOCOUTQ, &bytes), bytes < CLIENT_WRITE_BUF * 2);
		}
		else
			serverEvent->readReady = true;
	}
	if (!hasFlag(client->activeSegments, CS_CLIENT_OUT) && Queue_Count(client->commandQueue) == 0)
	{
		closeClient(client, CS_SERVER | CS_CLIENT);
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
		eventData->readReady = true;
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

void setup_args(int argc, char *argv[])
{
	serverArguments = parseArguments(argc, argv);

	char msg[1024];
	size_t msgLen;
	msgLen = get_proxy_addr(&serverArguments, msg);
	log(LOG_DEBUG, "%s", msg);
	msgLen = get_mgmt_addr(&serverArguments, msg);
	log(LOG_DEBUG, "%s", msg);
	msgLen = get_listen_port(&serverArguments, msg);
	log(LOG_DEBUG, "%s", msg);
	msgLen = get_origin_port(&serverArguments, msg);
	log(LOG_DEBUG, "%s", msg);
	msgLen = get_mgmt_port(&serverArguments, msg);
	log(LOG_DEBUG, "%s", msg);
}

void startServer()
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
	fcntl(epollfd, F_SETFD, FD_CLOEXEC);
	// Create collections
	evDataPool = Pool_Create(sizeof(EventData));
	clients = Pool_Create(sizeof(ClientData));

	// Add passive socket to epoll

	int tcpSockets[2];
	int count = setupTCPServerSocket(serverArguments.listenAddr, serverArguments.listenPort, tcpSockets);
	if (count <= 0)
		log(LOG_FATAL, "Cannot open TCP socket");
	for (int i = 0; i < count; i++)
		tcpEvents[i] = eventAdd(
		    (EventData){
		        .fdrw = tcpSockets[i],
		        .type = ST_PASSIVE_TCP,
		    },
		    EPOLLIN);

	int udpSockets[2];
	count = setupUDPServerSocket(serverArguments.mgmtAddr, serverArguments.mgmtPort, udpSockets);
	if (count <= 0)
		log(LOG_FATAL, "Cannot open UDP socket");
	for (int i = 0; i < count; i++)
		udpEvents[i] = eventAdd(
		    (EventData){
		        .fdrw = udpSockets[i],
		        .type = ST_PASSIVE_UDP,
		    },
		    EPOLLIN);

	int sigfd = signalfd_setup();
	fcntl(sigfd, F_SETFD, FD_CLOEXEC);
	// Add signalfd to epoll
	eventAdd(
	    (EventData){
	        .fdrw = sigfd,
	        .type = ST_SIGNAL,
	    },
	    EPOLLIN);

	if (resolve_dns("localhost", "2110", 0))			//TODO: de donde sale ese 2110?
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
			for (int i = 0; i < sizeof(tcpEvents) / sizeof(*tcpEvents); i++)
				if (tcpEvents[i] >= 0)
				{
					EventData *ev = Pool_GetRef(evDataPool, tcpEvents[i]);
					close(ev->fdrw);
				}
			for (int i = 0; i < sizeof(udpEvents) / sizeof(*udpEvents); i++)
				if (udpEvents[i] >= 0)
				{
					EventData *ev = Pool_GetRef(evDataPool, udpEvents[i]);
					close(ev->fdrw);
				}

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
		for (int i = 0; i < sizeof(tcpEvents) / sizeof(*tcpEvents); i++)
			if (tcpEvents[i] >= 0)
			{
				EventData *ev = Pool_GetRef(evDataPool, tcpEvents[i]);
				if (!ev->readReady)
					continue;

				bool pending = !acceptClient(ev->fdrw);
				ev->readReady = pending;
				advanced |= pending;
			}

		for (int i = 0; i < sizeof(udpEvents) / sizeof(*udpEvents); i++)
			if (udpEvents[i] >= 0)
			{
				EventData *ev = Pool_GetRef(evDataPool, udpEvents[i]);
				if (!ev->readReady)
					continue;

				struct sockaddr_storage clntAddr; // Client address
				socklen_t clntAddrLen = sizeof(clntAddr);
				static char udpBuffer[128];
				ssize_t numBytesRcvd =
				    recvfrom(ev->fdrw, udpBuffer, sizeof(udpBuffer) - 1, 0, (struct sockaddr *)&clntAddr, &clntAddrLen);
				if (numBytesRcvd < 0 && errno == EWOULDBLOCK)
				{
					ev->readReady = false;
					break;
				}
				udpBuffer[numBytesRcvd] = 0;
				processCmd(udpBuffer, numBytesRcvd, ev->fdrw, (struct sockaddr *)&clntAddr, clntAddrLen,
				           &serverArguments);
				advanced = true;
			}

		int clientCount = Pool_Count(clients);
		if (clientCount)
		{
			Handle activeClients[clientCount];
			Pool_ToIndexArray(clients, activeClients);
			for (int i = 0; i < clientCount; i++)
				advanced |= processClient(Pool_GetRef(clients, activeClients[i]));
		}
	}
}