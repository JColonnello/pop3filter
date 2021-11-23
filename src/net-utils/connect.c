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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

bool resolve_address(const char *address, const char *port,
                     struct addrinfo **addrinfo);

int startTCPConnection(const char *hostname, const char *port) {
  int ret;

  struct addrinfo *servAddr;
  if (!resolve_address(hostname, port, &servAddr)) {
    perror("Unable to resolve address");
    // TODO handle error
    return -1;
  }

  printf(" Trying to connect to %s:%s\n", hostname, port);

  int connSock = -1;
  for (struct addrinfo *addr = servAddr; addr != NULL && connSock == -1;
       addr = addr->ai_next) {

    connSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (connSock >= 0) {
      goto finalize;
    }

    ret = connect(connSock, servAddr->ai_addr, servAddr->ai_addrlen);
    if (ret == 0) {
      goto finalize;
    }
  }
  printf(" Can't connect. Check if the server is working property.\n");
  close(connSock);
  connSock = -1;
finalize:
  return connSock;
}

// enum ConnectionResult connectToServer(int task, int *fd){

// }

bool resolve_address(const char *address, const char *port,
                     struct addrinfo **addrinfo) {

  struct addrinfo addr = {
      .ai_family = AF_UNSPEC, /* Allow IPv4 or IPv6 */
      .ai_socktype = SOCK_STREAM,
      .ai_flags = AI_PASSIVE, /* For wildcard IP address */
      .ai_protocol = IPPROTO_TCP,
      .ai_canonname = NULL,
      .ai_addr = NULL,
      .ai_next = NULL,
  };

  if (0 != getaddrinfo(address, port, &addr, addrinfo)) {
    return false;
  }
  return true;
}