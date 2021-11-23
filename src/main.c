#include "logger.h"
#include "server.h"

int main(int argc, char *argv[])
{
	
	setLogLevel(LOG_DEBUG);
	setup_args(argc, argv);
	startServer();
	processingLoop();

	return 0;
}