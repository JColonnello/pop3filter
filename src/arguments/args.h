#ifndef ARGS_H
#define ARGS_H

#include <netinet/ip.h>
#include <stdbool.h>

typedef struct
{
	char *logFile;
	char *mgmtAddr;
	char *mgmtPort;
	char *listenAddr;
	char *listenPort;
	int dns_timer;
	char *originPort;
	char *filterCmd;
	char *originServer;
} ServerArguments;

/**
 * Interpreta la linea de comandos (argc, argv) llenando
 * args con defaults o la seleccion humana. Puede cortar
 * la ejecución.
 */

extern ServerArguments serverArguments;
ServerArguments parseArguments(int argc, char *argv[]);
#endif
