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

int signalfd_setup(void) {
    int sfd;
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL); //we block the signal
    sfd = signalfd(-1, &mask, 0);
    //add it to the event queue
}

return signalfd_read(int fd) {
    ssize_t s;
    struct signalfd_siginfo fdsi;
    struct gaicb *host;

    while((s = read(fd, &fdsi, sizeof(struct signalfd_siginfo))) > 0){
    if (s != sizeof(struct signalfd_siginfo)) {
        printf("ERROR handling the signal");
        return;
    }
    host = fdsi.ssi_ptr; //the pointer passed to the sigevent structure
    //the result is in the host->ar_result member
    return connectHost(host);
    
}


int resolve_dns(int async) {
    struct addrinfo *request;
    request = calloc(1, sizeof(struct addrinfo));
    request->ai_family = AF_UNSPEC; //we dont care if its v4 or v6
    request->ai_socktype = SOCK_STREAM;
    request->ai_flags = AI_PASSIVE;
    request->ai_protocol= IPPROTO_TCP;
    //every other field is NULL-d by calloc

    struct gaicb *host;     //THE ELEMENTS CORRESPOND TO THE ARGUMENTS OF getaddrinfo
    host = calloc(1, sizeof(struct gaicb));
    host->ar_service = &port; //the port we will listen on
    host->ar_request = request;
    host->ar_name = &hostname;

    struct sigevent sig;
    sig.sigev_notify = SIGEV_SIGNAL;
    sig.sigev_value.sival_ptr = host;
    sig.sigev_signo = SIGRTMIN;

    int flag;
    async ? flag = GAI_NOWAIT : flag = GAI_WAIT;
    if(getaddrinfo_a(flag) != 0){
        perror("Unable to resolve address");
        return -1;
    }
    int sfd = signalfd_setup();
    if(sfd == -1){
        perror("Unable to setup the signal file descriptor");
        return -1;
    }
    return sfd;
}


int connectHost(struct gaicb * host){
    struct addrinfo *rp, *result;
    int fd= -1;

    result = host->ar_result;
    for(rp = result; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        bind(fd, rp->ai_addr, rp->ai_addrlen);
        listen(fd, SOMAXCONN);
        //error checks are missing!

        freeaddrinfo(host->ar_request);
        freeaddrinfo(result);
        //you should free everything you put into the gaicb
    }

    return fd;
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
