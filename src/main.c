#include "server.h"

int main(int argc, char *argv[])
{
	/// TODO: Parse arguments
	setup_args(argc, argv);
	startServer();
	processingLoop();

	return 0;
}