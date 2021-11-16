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
#include "net-utils/tcpUtils.h"

static int tcpSocket;
static int epollfd;
static bool sigioPending = false;

static Queue *eventQueue;
static Pool *evDataPool;
static Pool *clients;

typedef struct
{
	Handle clientEvent;
	Handle serverEvent;
	bool active;
} ClientData;

typedef enum
{
	ST_PASSIVE,
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

static Handle eventMod(EventData eventData, uint32_t epollFlags)
{
	Handle eventId = Pool_Add(evDataPool, &eventData);
	if (eventId < 0)
		log(FATAL, "Out of memory");

	struct epoll_event ev = (struct epoll_event){
	    .events = epollFlags | EPOLLET,
	    .data.u32 = eventId,
	};
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, eventData.fd, &ev) == -1)
		log(FATAL, "epoll_ctl: conn_sock");

	return eventId;
}

static bool processEvent(struct epoll_event event)
{
	EventData *eventData = Pool_GetRef(evDataPool, event.data.u32);
	if (eventData->fd == tcpSocket && event.events & EPOLLIN)
	{
		int clientSocket = acceptTCPConnection(tcpSocket);
		// No pending connection
		if (clientSocket < 0)
		{
			eventData->readReady = false;
			return true;
		}

		ClientData clientData = {
		    .serverEvent = -1,
		    .active = true,
		};
		Handle clientId = Pool_Add(clients, &clientData);
		if (clientId < 0)
			log(FATAL, "Out of memory");

		// Add to epoll
		Handle clientEvent = eventMod(
		    (EventData){
		        .clientId = clientId,
		        .fd = clientSocket,
		        .type = ST_CLIENT,
		    },
		    EPOLLIN | EPOLLOUT);
		// Set backward reference
		ClientData *ref = Pool_GetRef(clients, clientId);
		ref->clientEvent = clientEvent;

		// Could have more clients waiting, reschedule
		eventData->readReady = true;
		return false;
	}
	else if (eventData->type)
	{
		if (event.events & EPOLLOUT)
			eventData->writeReady = true;
		if (event.events & EPOLLIN)
		{
			ClientData *client = Pool_GetRef(clients, eventData->clientId);
			EventData *serverEvent = Pool_GetRef(evDataPool, client->serverEvent);
			// If server is not ready for write, reschedule
			if (serverEvent != NULL && !serverEvent->writeReady)
				return false;

			bool exhausted;
			/// TODO: Process client stream;
			// Skip
			static char buffer[CLIENT_WRITE_BUF];
			ssize_t skip = read(eventData->fd, buffer, sizeof(buffer));
			if (skip <= 0)
			{
				// Close client
				close(eventData->fd);
				Pool_Remove(evDataPool, event.data.u32);
				client->clientEvent = -1;
				client->active = false;
				return true;
			}
			exhausted = (skip < CLIENT_WRITE_BUF);
			eventData->readReady = !exhausted;
			return exhausted;
			// perror("OOf");
			// log(ERROR, "%d", errno);
		}
		return true;
	}

	log(ERROR, "Unhandled event");
	return true;
}

static void sigioHandler(int signal)
{
	sigioPending = true;
}

void startServer(const char *port)
{
	// Close stdio
	close(STDIN_FILENO);
	open("/dev/null", O_WRONLY);

	tcpSocket = setupTCPServerSocket(port);
	if (tcpSocket < 0)
		log(FATAL, "Cannot open TCP socket");

	// Set signal handler for SIGIO
	struct sigaction handler = (struct sigaction){.sa_handler = sigioHandler};
	sigfillset(&handler.sa_mask);
	if (sigaction(SIGIO, &handler, NULL) < 0)
		log(FATAL, "sigaction() failed for SIGIO");
	// We must own the socket to receive the SIGIO message
	if (fcntl(tcpSocket, F_SETOWN, getpid()) < 0)
		fprintf(stderr, "Unable to set process owner to us");

	// Enable SIGIO
	sigset_t mask, oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SI_SIGIO);
	sigprocmask(SIG_UNBLOCK, &mask, &oldmask);

	// Create epoll fd
	epollfd = epoll_create1(0);
	if (epollfd == -1)
		log(FATAL, "Failed creating epoll handler");

	// Create collections
	evDataPool = Pool_Create(sizeof(EventData));
	eventQueue = Queue_Create(0, sizeof(struct epoll_event));
	clients = Pool_Create(sizeof(ClientData));

	// Add passive socket to epoll
	eventMod(
	    (EventData){
	        .fd = tcpSocket,
	        .type = ST_PASSIVE,
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
				log(FATAL, "epoll_pwait");
		}

		// Re-enable SIGIO
		sigprocmask(SIG_UNBLOCK, &sigioMask, NULL);

		// Add events to queue
		for (int i = 0; i < nfds; i++)
			Queue_Enqueue(eventQueue, &events[i]);

		// Process events
		for (int i = Queue_Count(eventQueue); i > 0; i--)
		{
			struct epoll_event event;
			Queue_Dequeue(eventQueue, &event);
			bool handled = processEvent(event);
			if (!handled)
				Queue_Enqueue(eventQueue, &event);
		}
	}
}