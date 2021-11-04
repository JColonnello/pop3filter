#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "udpUtils.h"


int setupUDPServerSocket(const char * service) {
	// Construct the server address structure
	struct addrinfo addrCriteria;					// Criteria for address
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;				// Any address family
	addrCriteria.ai_flags = AI_PASSIVE;				// Accept on any address/port
	addrCriteria.ai_socktype = SOCK_DGRAM;			// Only datagram socket
	addrCriteria.ai_protocol = IPPROTO_UDP;			// Only UDP socket

	struct addrinfo *servAddr;			// List of server addresses
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
	if (rtnVal != 0) {
		// TODO: Handle error
		// log(FATAL, "getaddrinfo() failed %s", gai_strerror(rtnVal))
	}

	int servSock = -1;
	// Intentamos ponernos a escuchar en alguno de los puertos asociados al servicio, sin especificar una IP en particular
	// Iteramos y hacemos el bind por alguna de ellas, la primera que funcione, ya sea la general para IPv4 (0.0.0.0) o IPv6 (::/0) .
	// Con esta implementación estaremos escuchando o bien en IPv4 o en IPv6, pero no en ambas
	for (struct addrinfo *addr = servAddr; addr != NULL && servSock == -1; addr = addr->ai_next) {
		//errno = 0;
		// Create UDP socket
		servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (servSock < 0) {
			// log(DEBUG, "Cant't create socket on %s : %s ", printAddressPort(addr, addrBuffer), strerror(errno));	 
			continue;		// Socket creation failed; try next address
		}

		// Bind to ALL the address
		if (bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0 ) {
			// Print local address of socket
			struct sockaddr_storage localAddr;
			socklen_t addrSize = sizeof(localAddr);
			if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) >= 0) {
				//printSocketAddress((struct sockaddr *) &localAddr, addrBuffer);
				// log(INFO, "Binding to %s", addrBuffer);
			}
		} else {
			// log(DEBUG, "Cant't bind %s", strerror(errno));  
			close(servSock);  // Close and try with the next one
			servSock = -1;
		}
	}

	freeaddrinfo(servAddr);

	return servSock;
}
