#include <asm-generic/errno.h>
#define _GNU_SOURCE
#include "connect.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "net-utils/tcpUtils.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
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

struct gaicb host;

int signalfd_setup()
{
	int sfd;
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGRTMIN);
	sigprocmask(SIG_BLOCK, &mask, NULL); // we block the signal
	sfd = signalfd(-1, &mask, 0);
	fcntl(sfd, F_SETFD, FD_CLOEXEC);
	// add it to the event queue
	return sfd;
}

int connectHost()
{
	struct addrinfo *rp;
	int ret;
	int connSock = -1;

	for (rp = host.ar_result; rp != NULL; rp = rp->ai_next)
	{
		connSock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (connSock < 0)
		{
			continue;
		}

		int buffer = CLIENT_WRITE_BUF * 2;
		size_t optSize = sizeof(buffer);
		fcntl(connSock, F_SETFL, O_NONBLOCK);
		fcntl(connSock, F_SETFD, FD_CLOEXEC);
		setsockopt(connSock, SOL_SOCKET, SO_SNDBUF, &buffer, optSize);
		buffer = CLIENT_WRITE_BUF;
		setsockopt(connSock, SOL_SOCKET, SO_SNDLOWAT, &buffer, optSize);

		ret = connect(connSock, rp->ai_addr, sizeof(*rp->ai_addr));
		if (ret == 0 || (ret == -1 && errno == EINPROGRESS))
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

	s = read(fd, &fdsi, sizeof(struct signalfd_siginfo));
	if (s != sizeof(struct signalfd_siginfo))
	{
		printf("ERROR handling the signal");
		return -1;
	}
	// the result is in the host.ar_result member
	// return connectHost(host);
	free((char *)host.ar_name);
	free((char *)host.ar_service);
	free((void *)host.ar_request);

	return 0;
}

void free_dns()
{
	freeaddrinfo(host.ar_result);
}

int resolve_dns(char *hostname, char *port, int async)
{
	struct addrinfo *request;
	request = calloc(1, sizeof(struct addrinfo));
	request->ai_family = AF_UNSPEC; // we dont care if its v4 or v6
	request->ai_socktype = SOCK_STREAM;
	request->ai_flags = AI_PASSIVE;
	request->ai_protocol = IPPROTO_TCP;
	// every other field is NULL-d by calloc

	host = (struct gaicb){0};
	host.ar_service = strdup(port); // the port we will listen on
	host.ar_request = request;
	host.ar_name = strdup(hostname);

	struct sigevent sig;
	sig.sigev_notify = SIGEV_SIGNAL;
	sig.sigev_value.sival_ptr = &host;
	sig.sigev_signo = SIGRTMIN;

	int flag = async ? GAI_NOWAIT : GAI_WAIT;
	struct gaicb *reqs[1] = {&host};
	if (getaddrinfo_a(flag, reqs, 1, &sig) != 0)
	{
		perror("Unable to resolve address");
		return -1;
	}
	if (!async)
	{
		free((char *)host.ar_name);
		free((char *)host.ar_service);
		freeaddrinfo((void *)host.ar_request);
	}
	return 0;
}
