#define _GNU_SOURCE
#include "connect.h"
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>
#include <netdb.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>

void signalfd_setup(void) {
    int sfd;
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL); //we block the signal
    sfd = signalfd(-1, &mask, 0);
    //add it to the event queue
}

int startTCPConnection(const char *hostname, const char *port){
    int ret;
    struct gaicb *host;
    struct addrinfo *hints;
    struct sigevent sig;
    struct sockaddr_in serv_addr;
    host = calloc(1, sizeof(struct gaicb));
    hints = calloc(1, sizeof(struct addrinfo));

    hints->ai_family = AF_UNSPEC; //we dont care if its v4 or v6
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_protocol= IPPROTO_TCP;
    //every other field is NULL-d by calloc

    host->ar_service = &port; //the port we will listen on
    host->ar_request = hints;
    host->ar_name = &hostname;

    sig.sigev_notify = SIGEV_SIGNAL;
    sig.sigev_value.sival_ptr = host;
    sig.sigev_signo = SIGRTMIN;

    uint16_t portUnit16 = (uint16_t) ~((unsigned int) atoi(port));
    serv_addr.sin_family = AF_UNSPEC;
    serv_addr.sin_port = htons(portUnit16);
    
    if(0 != getaddrinfo_a(GAI_NOWAIT, &host, 1, &sig)) {
        perror("Unable to resolve address");
        goto connection_failed;
    }
    signalfd_setup();

    printf(" Trying to connect to %s:%s\n", hostname, port);
    
    int connSock = -1;
	for (struct addrinfo *addr = hints; addr != NULL && connSock == -1; addr = addr->ai_next) {
        
        connSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if (connSock < 0)
        {
            continue;
        }

        ret = connect(connSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (ret == 0)
        {
            goto finalize;
        }
            
    }
connection_failed:
    printf(" Can't connect. Check if the server is working property.\n");
    close(connSock);
    connSock = -1;
finalize:
    return connSock;
    
    
}


enum ConnectionResult connectToServer(int task, int *fd){

}
