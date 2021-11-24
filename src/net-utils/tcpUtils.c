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
#include "stats.h"

#define MAXPENDING 64 // Maximum outstanding connection requests
#define MAX_ADDR_BUFFER 128

static int setupSocket(struct addrinfo addrCriteria, char * host, const char *service, int *fd)
{
	struct addrinfo *servAddr;                                         // List of server addresses
	int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddr); // Lista de ips del servidor en servAddr
	if (rtnVal != 0)
	{
		// getaddrinfo failed
		return -1;
	}

	int fdCount = 0;
	// Intentamos ponernos a escuchar en alguno de los puertos asociados al servicio, sin especificar una IP en
	// particular Iteramos y hacemos el bind por alguna de ellas, la primera que funcione, ya sea la general para IPv4
	// (0.0.0.0) o IPv6 (::/0) . Con esta implementación estaremos escuchando o bien en IPv4 o en IPv6, pero no en ambas
	for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next)
	{
		errno = 0;
		// Create a TCP socket
		int servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (servSock < 0)
		{
			// no se pudo crear el socket
			// Se prueba con el siguiente en la lista
			continue; // Socket creation failed; try next address
		}
		fcntl(servSock, F_SETFD, FD_CLOEXEC);
		int opt = 1;
		if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
			goto error;
		}

		opt = 1;
		if (addr->ai_family == AF_INET6 &&
		    setsockopt(servSock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
			goto error;
		}

		// Set nonblocking
		fcntl(servSock, F_SETFL, O_NONBLOCK);
		// We must own the socket to receive the SIGIO message
		if (fcntl(servSock, F_SETOWN, getpid()) < 0)
			fprintf(stderr, "Unable to set process owner to us");

		// Bind to ALL the address and set socket to listen
		if (bind(servSock, addr->ai_addr, addr->ai_addrlen) < 0)
			goto error;
		else
		{
			// // Print local address of socket
			// struct sockaddr_storage localAddr;
			// socklen_t addrSize = sizeof(localAddr);
			// if (getsockname(servSock, (struct sockaddr *)&localAddr, &addrSize) >= 0)
			// {
			// 	// bindeando
			// }
		};

		fd[fdCount++] = servSock;
		continue;
	error:
		// Manejo de error, fallo en bind
		close(servSock); // Close and try with the next one
	}

	freeaddrinfo(servAddr);

	return fdCount;
}

int setupTCPServerSocket(char * host, const char *service, int *fd)
{
	// Construct the server address structure
	struct addrinfo addrCriteria;                   // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;             // Any address family
	addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
	addrCriteria.ai_socktype = SOCK_STREAM;         // Only stream sockets
	addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

	int socks[2];
	int j = 0, count = setupSocket(addrCriteria, host, service, socks);
	for (int i = 0; i < count; i++)
	{
		if (listen(socks[i], MAXPENDING) >= 0)
			fd[j++] = socks[i];
		else
			close(socks[i]);
	}
	return j;
}

int setupUDPServerSocket(char * host, const char *service, int *fd)
{
	// Construct the server address structure
	struct addrinfo addrCriteria;                   // Criteria for address
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;             // Any address family
	addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
	addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram socket
	addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP socket

	return setupSocket(addrCriteria, host, service, fd);
}

int acceptTCPConnection(int servSock)
{
	addCurrentConnection();
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
			log(LOG_FATAL, "accept failed");
	}

	// Set nonblocking and buffer sizes
	int buffer = CLIENT_WRITE_BUF * 2;
	size_t optSize = sizeof(buffer);
	fcntl(clntSock, F_SETFL, O_NONBLOCK);
	setsockopt(clntSock, SOL_SOCKET, SO_SNDBUF, &buffer, optSize);
	buffer = CLIENT_WRITE_BUF;
	setsockopt(clntSock, SOL_SOCKET, SO_SNDLOWAT, &buffer, optSize);

	// clntSock is connected to a client!
	return clntSock;
}
