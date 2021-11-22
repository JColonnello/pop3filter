#include "server.h"

int main(int argc, char *argv[])
{
	/// TODO: Parse arguments

	startServer("1234");
	processingLoop();

	return 0;
}