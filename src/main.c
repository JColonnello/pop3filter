#include "server.h"

int main(int argc, char *argv[])
{
	/// TODO: Parse arguments
	setup_args(argc, argv);
	startServer("1234");
	processingLoop();

	return 0;
}