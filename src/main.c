#include "server.h"

int main(int argc, char * argv[]) 
{
	///TODO: Parse arguments

	startServer(argv[1]);
	processingLoop();
	
	return 0;
}