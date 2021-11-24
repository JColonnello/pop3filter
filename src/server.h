#pragma once
#include "collections/pool.h"
#include "collections/queue.h"

typedef struct ClientData ClientData;
#include "pop3/pop3.h"

typedef enum
{
	CS_CLIENT_OUT = 0x1,
	CS_CLIENT_IN = 0x2,
	CS_CLIENT = CS_CLIENT_IN | CS_CLIENT_OUT,
	CS_SERVER_OUT = 0x4,
	CS_SERVER_IN = 0x8,
	CS_SERVER = CS_SERVER_IN | CS_SERVER_OUT,
	CS_FILTER_OUT = 0x10,
	CS_FILTER_IN = 0x20,
	CS_FILTER = CS_FILTER_IN | CS_FILTER_OUT,
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
	Input filterState;
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

void setup_args(int argc, char * argv[]);
void startServer();
void processingLoop();
