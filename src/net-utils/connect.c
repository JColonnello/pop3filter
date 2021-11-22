#define _GNU_SOURCE
#include "connect.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/types.h>

struct gaicb * host;        //TODO liberar recursos

int signalfd_setup()
{
	int sfd;
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGRTMIN);
	sigprocmask(SIG_BLOCK, &mask, NULL); // we block the signal
	sfd = signalfd(-1, &mask, 0);
	// add it to the event queue
	return sfd;
}

int connectHost()
{
    struct sockaddr_in serv_addr;
    uint16_t portUnit16 = (uint16_t) ~((unsigned int) atoi(host->ar_service));
    serv_addr.sin_family = AF_UNSPEC;
    serv_addr.sin_port = htons(portUnit16);
    
	struct addrinfo *rp;
    int ret;
	int connSock = -1;

	for (rp = host->ar_result; rp != NULL; rp = rp->ai_next)
	{
		connSock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (connSock < 0)
        {
            continue;
        }
        ret = connect(connSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (ret == 0)
        {
            return connSock;
        }
    }
    return -1;
}

int signalfd_read(int fd)
{
	ssize_t s;
	struct signalfd_siginfo fdsi;

	while ((s = read(fd, &fdsi, sizeof(struct signalfd_siginfo))) > 0)
	{
		if (s != sizeof(struct signalfd_siginfo))
		{
			printf("ERROR handling the signal");
			return -1;
		}
		host = (void *)fdsi.ssi_ptr; // the pointer passed to the sigevent structure
	}
	// the result is in the host->ar_result member
	//return connectHost(host);

    return 0;
}

int resolve_dns(char * hostname, char * port, int async)
{
	struct addrinfo *request;
	request = calloc(1, sizeof(struct addrinfo));
	request->ai_family = AF_UNSPEC; // we dont care if its v4 or v6
	request->ai_socktype = SOCK_STREAM;
	request->ai_flags = AI_PASSIVE;
	request->ai_protocol = IPPROTO_TCP;
	// every other field is NULL-d by calloc

	host = calloc(1, sizeof(struct gaicb));
	host->ar_service = strdup(port); // the port we will listen on
	host->ar_request = request;
	host->ar_name = strdup(hostname);

	struct sigevent sig;
	sig.sigev_notify = SIGEV_SIGNAL;
	sig.sigev_value.sival_ptr = host;
	sig.sigev_signo = SIGRTMIN;

	int flag = async ? GAI_NOWAIT : GAI_WAIT;
	if (getaddrinfo_a(flag, &host, 1, &sig) != 0)
	{
		perror("Unable to resolve address");
		return -1;
	}
    return 0;
}


