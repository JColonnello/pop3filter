#pragma once

#include <stdio.h>

#define BUFSIZE	 128
#define ECHO_MAX 100

typedef enum { WAITING, PARTIAL_ECHO, ECHO, DATE, TIME, BAD_LINE } LineStatus;

typedef enum { LOCALE_ES, LOCALE_EN, STATS, BAD_DATAGRAM } DatagramStatus;

typedef struct {
	char buf[BUFSIZE + 1], *lim, *cur, *mar, *text_s, *text_f;
	char writeBuf[BUFSIZE * 2];
	int state, cond;
	size_t echoCount, writeCount;
} Input;

void initState(Input *in);
size_t processTcpInput(Input *in, int fd, size_t maxInput);
