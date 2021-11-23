#ifndef TCPSERVERUTIL_H_
#define TCPSERVERUTIL_H_

#include <stdio.h>
#include <sys/socket.h>

#define CLIENT_WRITE_BUF 1024

// Create, bind, and listen a new TCP server socket
int setupTCPServerSocket(const char *service, int *fd);

// Accept a new TCP connection on a server socket
int acceptTCPConnection(int servSock);

#endif
