#pragma once

typedef struct
{
	const char *logFile;
	const struct sockaddr *listenAddr;
	const struct sockaddr *mgmtAddr;
	const char *mgmtPort;
	const struct sockaddr *listenPort;
	const char *originPort;
	const char *filterCmd;
	const char *originServer;
} ServerArguments;
