#pragma once
#include "collections/queue.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef enum
{
	POP_INVALID,
	POP_INCOMPLETE,
	POP_UNKNOWN,

	POP_USER,
	POP_PASS,
	POP_QUIT,
	POP_STAT,
	POP_LIST,
	POP_LIST_MSG,
	POP_RETR,
	POP_DELE,
	POP_NOOP,
	POP_RSET,
	POP_CAPA,
} PopCommand;

typedef struct
{
	PopCommand cmd;
	char *data;
	size_t len;
} PendingRequest;

#include "../parsers/pop3.h"
#include "server.h"

bool copyMsg(Input *in, int to, bool stuff);
bool processPopClient(Input *in, int fd, Queue *queue, char **user);
ssize_t sendPopRequest(int fd, PendingRequest request);
int processPopServer(ClientData *client, int clientfd, bool *redirect);
void initState(Input *in);
ssize_t fillBuffer(Input *in, int fd);
