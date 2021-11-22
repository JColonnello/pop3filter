#ifndef ARGS_H
#define ARGS_H

#include <netinet/ip.h>
#include <stdbool.h>

typedef struct
{
	char *logFile;
	struct sockaddr_in mgmtSock;
	struct sockaddr_in listenSock;

	char *originPort;
	char *filterCmd;
	char *originServer;
} ServerArguments;

extern ServerArguments serverArguments;
ServerArguments parseArguments(int argc, char *argv[]);
#endif
