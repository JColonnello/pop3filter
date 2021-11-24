#include "functions.h"
#include "logger.h"
#include "server.h"
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	ServerArguments args;
	memset(&args, 0, sizeof(ServerArguments));
	setLogLevel(LOG_DEBUG);
	setup_args(argc, argv);
	startServer();
	processingLoop();

	return 0;
}