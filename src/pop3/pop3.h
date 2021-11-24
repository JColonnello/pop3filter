#pragma once
#include "collections/queue.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct
{
	char *lim, *cur, *mar, *tok;
	char *buf, *writeBuf;
	int state, cond;
	size_t written, bufSize;
} Input;

typedef enum
{
	POP_INVALID,
	POP_INCOMPLETE,
	POP_UNKNOWN,
	POP_USER,
	POP_RETR,
} PopCommand;

typedef struct
{
	PopCommand cmd;
	char *data;
	size_t len;
} PendingRequest;

#include <server.h>

bool copyMsg(Input *in, int to, bool stuff);
bool processPopClient(Input *in, int fd, Queue *queue, char **user);
ssize_t sendPopRequest(int fd, PendingRequest request);
bool processPopServer(ClientData *client, int clientfd, bool *redirect);
void initState(Input *in);
ssize_t fillBuffer(Input *in, int fd);
