#include "pop3.h"
#include "collections/queue.h"
#include "net-utils/tcpUtils.h"
#include "parsers/pop3.h"
#include "server.h"
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

static ssize_t fill(Input *in, int fd)
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
	in->lim += count;
	*in->lim = 0;

	return count;
}

bool copyMsg(Input *in, int from, int to, int *count, bool stuff)
{
	*count = fill(in, from);
	bool result = stuff ? stuffMail(in) : destuffMail(in);
	write(to, in->writeBuf, in->written);
	return result;
}

ssize_t processPopClient(Input *in, int fd, Queue *queue)
{
	ssize_t count = fill(in, fd);
	char *arg;
	size_t argLen, len;
	PopCommand cmd = parsePopRequest(in, &arg, &argLen, &len);
	switch (cmd)
	{
	case POP_INCOMPLETE:
		break;
	default: {
		char *data = malloc(len + 1);
		memcpy(data, in->tok, len);
		data[len] = 0;
		PendingRequest req = {
		    .cmd = cmd,
		    .data = data,
		    .len = len,
		};
		Queue_Enqueue(queue, &req);
	}
	}
	return count;
}

ssize_t sendPopRequest(int fd, PendingRequest request)
{
	ssize_t count = write(fd, request.data, request.len);
	free(request.data);
	return count;
}

ssize_t processPopServer(ClientData *client, int serverfd, int clientfd, bool *redirect)
{
	Input *in = &client->responseState;
	Queue *queue = client->commandQueue;
	ssize_t count = fill(in, serverfd);
	PendingRequest req;
	Queue_Peek(queue, &req);
	switch (req.cmd)
	{
	case POP_UNKNOWN:
	case POP_INVALID: {
		char msg[] = "-ERR Invalid command";
		write(clientfd, msg, sizeof(msg));
		Queue_Dequeue(queue, NULL);
		break;
	}
	case POP_INCOMPLETE:
	case POP_USER: {
		bool completed = parsePopResponse(in, false);
		if (completed)
		{
			write(clientfd, in->writeBuf, in->written);
			in->written = 0;
			Queue_Dequeue(queue, NULL);
			client->pending = false;
			break;
		}
		else
			return count;
	}
	}
	return count;
}