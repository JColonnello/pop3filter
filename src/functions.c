#include "functions.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "logger.h"
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// con fprintf(stderr, "ERROR MESSAGE"); escribo el file
size_t set_error_file(ServerArguments * args, char * file, char * msg)
{
	args->logFile = file;
	/*int out = fopen(file,"r+");
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
	}*/
	return sprintf(msg, "Setting error file\n");
}

size_t get_error_file(ServerArguments * args, char * msg)
{
	return sprintf(msg, "Error file is: %s\n", args->logFile);
}

size_t set_proxy_addr(ServerArguments * args, char * addr, char * msg) {
	args->listenAddr = addr;
	//inet_pton(AF_INET, addr, &(args->listenSock.sin_addr));
	return sprintf(msg, "Setting proxy address\n");
	
}
size_t get_proxy_addr(ServerArguments * args, char * msg){
	//char *proxy_addr = inet_ntoa(args->listenSock.sin_addr);
	return sprintf(msg, "Proxy address is: %s\n", args->listenAddr);
}
size_t set_listen_port(ServerArguments * args, char * port, char * msg){
	args->listenPort = port;
	//args->listenSock.sin_port = htons(atoi(port));
	return sprintf(msg, "Setting listen port\n");
}
size_t get_listen_port(ServerArguments * args, char * msg){

	//unsigned short port =ntohs(args->listenSock.sin_port); 
	return sprintf(msg, "Listen port is: %s\n", args->listenPort);
}
size_t set_origin_port(ServerArguments * args, char * port, char * msg){
	args->originPort = port;
	return sprintf(msg, "Setting origin port\n");
}
size_t get_origin_port(ServerArguments * args, char * msg){
	//char *port = args->originPort;
	return sprintf(msg, "Origin port is: %s\n", args->originPort);
}
/*
size_t set_filter(ServerArguments * args, char * filter, char * msg){ 
	args->filterCmd = filter;
	return sprintf(msg, "Setting filter\n");

}*/
size_t set_mgmt_addr(ServerArguments * args, char * addr, char * msg){
	args->mgmtAddr = addr;
	//inet_pton(AF_INET, addr, &(args->mgmtSock.sin_addr));
	return sprintf(msg, "Setting management address\n");
}
size_t get_mgmt_addr(ServerArguments * args, char * msg){
	//char *mgmt_addr = inet_ntoa(args->mgmtSock.sin_addr);
	return sprintf(msg, "Management address is: %s\n", args->mgmtAddr);	
}
size_t set_mgmt_port(ServerArguments * args, char * port, char * msg){
	args->mgmtPort = port;
	//args->mgmtSock.sin_port = htons(atoi(port));
	return sprintf(msg, "Setting management port\n");
}
size_t get_mgmt_port(ServerArguments * args, char * msg){
	
	//unsigned short port = ntohs(args->mgmtSock.sin_port);
	return sprintf(msg, "Management port is: %s\n", args->mgmtPort);
}
size_t get_error(char * msg){
	return sprintf(msg, "Unknown command\n");
}

size_t set_filter(ServerArguments * args, char * filter, char * msg, char** enviroment){
	args->filterCmd = filter;
	pid_t pid;
	int fd[2];
	int fd2[2];
	if(pipe(fd) == -1)
    {
		log(LOG_DEBUG, "Error creating pipe\n");
       fprintf(stderr, "Error creating pipe\n");
    }
	if(pipe(fd2) == -1)
    {
		log(LOG_DEBUG, "Error creating pipe\n");
       fprintf(stderr, "Error creating pipe\n");
    }
	if((pid = fork()) < 0) {
		log(LOG_DEBUG, "set filter 1");
		return sprintf(msg, "Fork error: %d\n",strerror(errno));
	} else if (pid == 0) { //Child process
		dup2(fd[1],1); 
		dup2(fd2[0],0); 
		close(fd[1]);
		close(fd[0]);
		close(fd2[1]);
		close(fd2[0]);
		int ret = execle("/bin/sh", "sh", "-c", args->filterCmd, NULL,  enviroment);
		if(ret == -1){
			log(LOG_DEBUG, "set filter 2");
			
			perror(errno);
			return sprintf(msg, "execle() failed%d\n",strerror(errno));
		}
		

	} else{ //Father process
		close(fd[1]);
		close(fd2[0]);
	}
	log(LOG_DEBUG, "set filter 3");
	return sprintf(msg, "Setting filter\n");

}


size_t get_help(char * msg){
	//TODO: ESCRIBIR TEXTO DE AYUDA
	return sprintf(msg, "Options: \n"
	"-e: Especifica el archivo donde se redirecciona stderr de las ejecuciones de los filtros \n"  
	"-h: Imprime la ayuda\n"  
	"-l: Establece la direccion donde sirve el proxy\n"  
	"-L: Establece la direccion donde sirve el servicio de monitoreo\n"  
	"-o: Puerto donde se encuentra el servidor de management\n"  
	"-p: Puerto TCP donde escuchara por conexiones entrantes POP3\n"  
	"-P: Puerto TCP donde se encuentra el servidor POP3 en el servidor origen\n"  
	"-t: Comando utilizado para las transformaciones externas\n"  
	"-v: Imprime la informacion sobre la version\n"  
	);

}
size_t get_version(char * msg)
{
	return sprintf(msg, "POP3Filter 0.1\n"
	                "ITBA Protocolos de Comunicación 2021/2 -- Grupo 9\n");
}






