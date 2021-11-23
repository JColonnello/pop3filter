#include "logger.h"
#include "server.h"

int main(int argc, char *argv[])
{
	/// TODO: Parse arguments

	setLogLevel(LOG_DEBUG);
	startServer("1234");
	processingLoop();

	return 0;
}