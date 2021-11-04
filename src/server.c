#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <netinet/in.h>

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "net-utils/tcpUtils.h"
#include "logger.h"
#include "parsers/lexer.h"

#define MAX_CLIENTS 511
#define MAX_INPUT 1024
#define DEFAULT_PORT "9999"

#define MAX(x,y) ((x) > (y)) ? (x) : (y)

typedef struct
{
	int socket;
	Input tcpState;
} ClientData;

static ClientData *clients[MAX_CLIENTS];
static int socketTCP;

void startServer(const char *port)
{
	log(INFO, "Arranco");

	if(port == NULL)
		port = DEFAULT_PORT;

	log(DEBUG, "Listen on port: %s", port);
	socketTCP = setupTCPServerSocket(port);
	if(socketTCP < 0)
	{
		log(ERROR, "Cannot open TCP socket");
		exit(1);
	}

	memset(clients, 0, sizeof(clients));
}

void processingLoop()
{
	fd_set readfds, writefds;

	for(;;)
	{
		FD_ZERO(&readfds); 
		FD_ZERO(&writefds); 
		FD_SET(socketTCP, &readfds);
		int max_sd = socketTCP;			 //Necesito el maximo para el select

		//Add clients
		for(int i = 0; i < MAX_CLIENTS; i++) { 
			if(clients[i] == NULL)
				continue;

			int sd = clients[i]->socket;
			FD_SET(sd, clients[i]->tcpState.writeCount > 0 ? &writefds : &readfds);
			max_sd = MAX(max_sd, sd);
		}

		//Espero la actividad de algun sockdet
		int ready = select(max_sd + 1, &readfds, &writefds, NULL, NULL);   
		if(ready < 0 && (errno!=EINTR)) {
			log(ERROR, "select() failed: %s", strerror(errno));
		}

		//New TCP connections
		if(FD_ISSET(socketTCP, &readfds)) {
			log(INFO, "Entro en FD_ISSET");

			for(int i = 0; i < MAX_CLIENTS; i++){
				if(clients[i] == NULL) {
					int newSocket = acceptTCPConnection(socketTCP);
					if(newSocket < 0)
						break;

					clients[i] = malloc(sizeof(ClientData));
					if(clients[i]) {} //TODO: manejar error

					clients[i]->socket = newSocket;
					initState(&clients[i]->tcpState);
					break;
				}
			}
		}

		//TCP Clients
		for(int i = 0; i < MAX_CLIENTS; i++){
			ClientData *client = clients[i];
			bool closeClient = false;

			if(client == NULL)
				continue;

			int sd = client->socket;
			if(FD_ISSET(sd, &readfds)){
				size_t valread = processTcpInput(&client->tcpState, client->socket, MAX_INPUT);
				if(valread == 0)
					closeClient = true;
			}
			else if(FD_ISSET(sd, &writefds))
			{
				size_t count = client->tcpState.writeCount;
				char *buffer = client->tcpState.writeBuf;
				size_t sent = write(client->socket, buffer, count);
				if(sent < 0)
					closeClient = true;
				else if(sent < count)
					memmove(buffer, &buffer[sent], count - sent);
				client->tcpState.writeCount -= sent;
			}

			if(closeClient)
			{
				close(sd);
				free(client);
				clients[i] = NULL;
				log(INFO, "Closed client: %d", i);
			}
		}
	}
	return;

}