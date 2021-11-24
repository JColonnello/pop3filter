#pragma once
#include "collections/pool.h"
#include "collections/queue.h"

typedef struct ClientData ClientData;
#include "pop3/pop3.h"

typedef enum
{
	CS_CLIENT_SERVER = 1,
	CS_SERVER_CLIENT = 2,
	CS_SERVER_FILTER = 4,
	CS_FILTER_CLIENT = 8,
} CommSegment;

struct ClientData
{
	int id;
	Handle clientEvent;
	Handle serverEvent;
	Handle filterEvent;
	CommSegment activeSegments;
	Input responseState;
	Input requestState;
	Queue *commandQueue;
	bool pending;
	char *user;
};

typedef enum
{
	ST_PASSIVE_TCP,
	ST_PASSIVE_UDP,
	ST_CLIENT,
	ST_SERVER,
	ST_SIGNAL,
	ST_FILTER,
} SocketType;

typedef struct
{
	int fdrw;
	int fdw;
	Handle clientId;
	SocketType type;
	bool readReady, writeReady;
} EventData;

void startServer(const char *port);
void processingLoop();
