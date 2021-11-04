#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexer.h"
#include "cmd.h"
#include "../logger.h"

void initState(Input *in)
{
	*in = (Input) {0};
	in->cur = in->mar = in->lim = in->buf + BUFSIZE;
	in->state = -1;
	in->cond = yycinit;
	*in->lim = '\r';
}

static size_t fill(Input *in, int fd)
{
	const char *start = in->mar;
	const size_t shift = start - in->buf;
	const size_t free = BUFSIZE - (in->lim - start);

	memmove(in->buf, start, BUFSIZE - shift);
	in->lim -= shift;
	in->cur -= shift;
	in->mar -= shift;

	const size_t count = read(fd, in->lim, free);
	in->lim += count;
	*in->lim = '\r';

	return count;
}

size_t processTcpInput(Input *in, int fd, size_t maxInput)
{
	LineStatus status;
	size_t bytesRead = fill(in, fd);;

	for(;;)
	{
		status = lexLines(in);
		switch (status)
		{
			case ECHO:
				//lineaCorrecta();
				//omited break
			case PARTIAL_ECHO:
			{
				log(DEBUG, "echo");

				size_t count = in->text_f - in->text_s;
				char *text = in->text_s;
				size_t max = ECHO_MAX - in->echoCount;
				count = (count > max) ? max : count;
				in->echoCount += count;

				memcpy(&in->writeBuf[in->writeCount], text, count);
				in->writeCount += count;
				if(status == ECHO)
				{
					count = 2;
					memcpy(&in->writeBuf[in->writeCount], "\r\n", count);
					in->writeCount += count;
					in->echoCount = 0;
				}
				break;
			}
			case WAITING:
				return bytesRead;
			case BAD_LINE:
				//lineaIncorrecta();
				break;
			case DATE:
			{
				//lineaCorrecta();
				size_t count = 12;
				char date[count+1];
				//getDate(date);
				memcpy(&in->writeBuf[in->writeCount], date, count);
				in->writeCount += count;
				break;
			}
			case TIME:
			{
				//lineaCorrecta();
				size_t count = 10;
				char time[count+1];
				//getTime(time);
				memcpy(&in->writeBuf[in->writeCount], time, count);
				in->writeCount += count;
				break;
			}
		}
	}
}
