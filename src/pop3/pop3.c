#include "pop3.h"
#include "collections/queue.h"
#include "net-utils/tcpUtils.h"
#include "parsers/pop3.h"
#include "server.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void initState(Input *in)
{
	in->tok = in->cur = in->mar = in->lim = in->buf + in->bufSize;
	in->state = -1;
	in->cond = 0;
	*in->lim = 0;
}

ssize_t fillBuffer(Input *in, int fd)
{
	const char *start = in->tok;
	const size_t shift = start - in->buf;
	const size_t free = in->bufSize - (in->lim - start);

	memmove(in->buf, start, in->bufSize - shift);
	in->lim -= shift;
	in->cur -= shift;
	in->mar -= shift;
	in->tok -= shift;

	const ssize_t count = read(fd, in->lim, free);
	if (count >= 0)
		in->lim += count;
	*in->lim = 0;

	return count;
}

bool copyMsg(Input *in, int to, bool stuff)
{
	bool result = stuff ? stuffMail(in) : destuffMail(in);
	write(to, in->writeBuf, in->written);
	in->written = 0;
	return result;
}

bool processPopClient(Input *in, int fd, Queue *queue, char **user)
{
	char *arg;
	size_t argLen, len;
	PopCommand cmd = parsePopRequest(in, &arg, &argLen, &len);
	switch (cmd)
	{
	case POP_INCOMPLETE:
		return false;
	case POP_INVALID: {
		PendingRequest req = {.cmd = cmd};
		in->tok = in->cur;
		Queue_Enqueue(queue, &req);
		break;
	}
	case POP_USER:
		if (*user)
			free(*user);
		*user = malloc(argLen + 1);
		memcpy(*user, arg, argLen);
		(*user)[argLen] = 0;
	default: {
		char *data = malloc(len + 1);
		memcpy(data, in->tok, len);
		data[len] = 0;
		PendingRequest req = {
		    .cmd = cmd,
		    .data = data,
		    .len = len,
		};
		in->tok = in->cur;
		Queue_Enqueue(queue, &req);
	}
	}
	return true;
}

ssize_t sendPopRequest(int fd, PendingRequest request)
{
	ssize_t count = write(fd, request.data, request.len);
	free(request.data);
	return count;
}

int processPopServer(ClientData *client, int clientfd, bool *redirect)
{
	Input *in = &client->responseState;
	Queue *queue = client->commandQueue;
	PendingRequest req;
	bool multiline = false;
	if (!Queue_Peek(queue, &req))
		return 0;
	switch (req.cmd)
	{
	case POP_INVALID: {
		char msg[] = "-ERR Invalid command\r\n";
		write(clientfd, msg, sizeof(msg));
		Queue_Dequeue(queue, NULL);
		return 1;
	}
	case POP_LIST:
	case POP_CAPA:
		multiline = true;
		break;
	case POP_LIST_MSG:
	case POP_RETR:
	case POP_UNKNOWN:
	case POP_INCOMPLETE:
	case POP_PASS:
	case POP_QUIT:
	case POP_STAT:
	case POP_DELE:
	case POP_NOOP:
	case POP_RSET:
	case POP_USER:
		multiline = false;
		break;
	}
	bool success = false;
	bool completed = parsePopResponse(in, multiline, &success);
	if (completed)
	{
		if (req.cmd == POP_CAPA)
		{
			in->written -= sizeof(".\r\n") - 1;
			write(clientfd, in->writeBuf, in->written);
			char str[] = "PIPELINING\r\n.\r\n";
			write(clientfd, str, sizeof(str) - 1);
		}
		else
			write(clientfd, in->writeBuf, in->written);
		in->written = 0;
		Queue_Dequeue(queue, NULL);
		client->pending = false;
		if (req.cmd == POP_RETR && success)
			*redirect = true;
		else if (req.cmd == POP_QUIT)
			return -1;

		return 1;
	}
	else
		return 0;
}