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

bool resolve_address(char *address, uint16_t port, struct addrinfo ** addrinfo);


int startTCPConnection(const char *hostname, const char *port){
    int ret;
    
    struct addrinfo *servAddr;
    if(!resolve_address(hostname, port, &servAddr)) {
        perror("Unable to resolve address");
        // TODO handle error
        goto connection_failed;
    }

    printf(" Trying to connect to %s:%d\n", hostname, port);
    
    int connSock = -1;
	for (struct addrinfo *addr = servAddr; addr != NULL && connSock == -1; addr = addr->ai_next) {
        
        connSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if (connSock >= 0)
        {
            goto finalize;
        }

        ret = connect(connSock, servAddr->ai_addr, servAddr->ai_addrlen);
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


bool
resolve_address(char *address, uint16_t port, struct addrinfo ** addrinfo) {

  struct addrinfo addr = {
          .ai_family    = AF_UNSPEC,    /* Allow IPv4 or IPv6 */
          .ai_socktype  = SOCK_STREAM,
          .ai_flags     = AI_PASSIVE,   /* For wildcard IP address */
          .ai_protocol  = IPPROTO_TCP,           1
          .ai_canonname = NULL,
          .ai_addr      = NULL,
          .ai_next      = NULL,
  };

  char buff[7];
  snprintf(buff, sizeof(buff), "%hu", port);
  if (0 != getaddrinfo(address, buff, &addr, addrinfo)){
    return false;
  }
  return true;
}