#include "logger.h"
#include "server.h"
#include "functions.h"
#include <string.h>
#include <unistd.h>



int main(int argc, char *argv[])
{
	ServerArguments args;
	memset(&args, 0, sizeof(ServerArguments));
	char * filter = "cat";
	args.filterCmd = filter;
	char ans[2048];
	int fd[2];
	char* username = "username";
	set_filter(&args, username, fd);

	write(fd[1],"hola\n", 6);
	
	
	read(fd[0], ans, sizeof(ans));
	log(LOG_DEBUG, ans);
	setLogLevel(LOG_DEBUG);
	setup_args(argc, argv);
	startServer();
	processingLoop();

	return 0;
}