#include "functions.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "logger.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// con fprintf(stderr, "ERROR MESSAGE"); escribo el file
size_t set_error_file(ServerArguments args, char *file, char * msg)
{
	args.logFile = file;
	int out = fopen(file,"r+");
	if (out == -1)
	{
		log(LOG_DEBUG, "ERROR OPENING FILE");
		return sprintf(msg, "Error setting file\n");
	}
	
	dup(fileno(stderr));
	if (-1 == dup2(out, fileno(stderr)))
	{
		log(LOG_DEBUG, "ERROR REDIRECTING STDERR");
		return sprintf(msg, "Error setting file\n");
	}
	return sprintf(msg, "Setting error file\n");
}

size_t get_error_file(ServerArguments args, char * msg)
{
	return sprintf(msg, "Error file is: %s\n", args.logFile);
}

size_t set_proxy_addr(ServerArguments args, char * addr, char * msg) {
	inet_pton(AF_INET, addr, &(args.listenSock.sin_addr));
	return sprintf(msg, "Setting proxy address\n");
	
}
size_t get_proxy_addr(ServerArguments args, char * msg){
	char *proxy_addr = inet_ntoa(args.listenSock.sin_addr);
	return sprintf(msg, "Proxy address is: %s\n", proxy_addr);
}
size_t set_listen_port(ServerArguments args, char * port, char * msg){
	args.listenSock.sin_port = htons(atoi(port));
	return sprintf(msg, "Setting listen port\n");
}
size_t get_listen_port(ServerArguments args, char * msg){
	unsigned short port = args.listenSock.sin_port;
	return sprintf(msg, "Listen port is: %d\n", port);
}
size_t set_origin_port(ServerArguments args, char * port, char * msg){
	args.originPort = port;
	return sprintf(msg, "Setting origin port\n");
}
size_t get_origin_port(ServerArguments args, char * msg){
	char *port = args.originPort;
	return sprintf(msg, "Origin port is: %s\n", port);
}

size_t set_filter(ServerArguments args, char * filter, char * msg){ 
	args.filterCmd = strdup(filter);
	return sprintf(msg, "Setting filter\n");

}
size_t set_mgmt_addr(ServerArguments args, char * addr, char * msg){
	inet_pton(AF_INET, addr, &(args.mgmtSock.sin_addr));
	return sprintf(msg, "Setting management address\n");
}
size_t get_mgmt_addr(ServerArguments args, char * msg){
	char *mgmt_addr = inet_ntoa(args.mgmtSock.sin_addr);
	return sprintf(msg, "Management address is: %s\n", mgmt_addr);	
}
size_t set_mgmt_port(ServerArguments args, char * port, char * msg){
	args.mgmtSock.sin_port = htons(atoi(port));
	return sprintf(msg, "Setting management port\n");
}
size_t get_mgmt_port(ServerArguments args, char * msg){
	unsigned short port = args.mgmtSock.sin_port;
	return sprintf(msg, "Management port is: %d\n", port);
}
size_t set_error(char * msg){
	return sprintf(msg, "Unknown command\n");
}






