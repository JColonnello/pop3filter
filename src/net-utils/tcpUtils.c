#include "tcpUtils.h"
#include "logger.h"
#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXPENDING 64 // Maximum outstanding connection requests
#define MAX_ADDR_BUFFER 128

int setupTCPServerSocket(const char *service)
{

	// Construct the server address structure
	struct addrinfo addrCriteria;                   // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;             // Any address family
	addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
	addrCriteria.ai_socktype = SOCK_STREAM;         // Only stream sockets
	addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

	struct addrinfo *servAddr;                                         // List of server addresses
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr); // Lista de ips del servidor en servAddr
	if (rtnVal != 0)
	{
		// getaddrinfo failed
		return -1;
	}

	int opt = 1;
	int servSock = -1;
	// Intentamos ponernos a escuchar en alguno de los puertos asociados al servicio, sin especificar una IP en
	// particular Iteramos y hacemos el bind por alguna de ellas, la primera que funcione, ya sea la general para IPv4
	// (0.0.0.0) o IPv6 (::/0) . Con esta implementación estaremos escuchando o bien en IPv4 o en IPv6, pero no en ambas
	for (struct addrinfo *addr = servAddr; addr != NULL && servSock == -1; addr = addr->ai_next)
	{
		errno = 0;
		// Create a TCP socket
		servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (servSock < 0)
		{
			// no se pudo crear el socket
			// Se prueba con el siguiente en la lista
			continue; // Socket creation failed; try next address
		}

		if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
			goto error;
		}

		// Set nonblocking
		fcntl(servSock, F_SETFL, O_NONBLOCK);

		// Bind to ALL the address and set socket to listen
		if ((bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0) && (listen(servSock, MAXPENDING) == 0))
		{
			// // Print local address of socket
			// struct sockaddr_storage localAddr;
			// socklen_t addrSize = sizeof(localAddr);
			// if (getsockname(servSock, (struct sockaddr *)&localAddr, &addrSize) >= 0)
			// {
			// 	// bindeando
			// }
		}
		else
			goto error;

		continue;
	error:
		// Manejo de error, fallo en bind
		close(servSock); // Close and try with the next one
		servSock = -1;
	}

	freeaddrinfo(servAddr);

	return servSock;
}

int acceptTCPConnection(int servSock)
{

	struct sockaddr_storage clntAddr; // Client address
	// Set length of client address structure (in-out parameter)
	socklen_t clntAddrLen = sizeof(clntAddr);

	// Wait for a client to connect
	int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
	if (clntSock < 0)
	{
		// No pending connections
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return -1;
		else
			log(FATAL, "accept failed");
	}

	/// TODO: Set nonblocking and buffer sizes
	int buffer = CLIENT_WRITE_BUF * 2;
	size_t optSize = sizeof(buffer);
	fcntl(clntSock, F_SETFL, O_NONBLOCK);
	setsockopt(clntSock, SOL_SOCKET, SO_SNDBUF, &buffer, optSize);
	buffer = CLIENT_WRITE_BUF;
	setsockopt(clntSock, SOL_SOCKET, SO_SNDLOWAT, &buffer, optSize);

	// clntSock is connected to a client!
	return clntSock;
}
