#ifndef MGMT_H
#define MGMT_H
#include "arguments/args.h"
#include <stddef.h>

typedef enum
{
	ERROR,
	STATS,
	SET_DNS_TIMER,
	GET_DNS_TIMER,
	SET_ERROR_FILE,
	GET_ERROR_FILE,
	SET_PROXY_ADDR,
	GET_PROXY_ADDR,
	SET_LISTEN_PORT,
	GET_LISTEN_PORT,
	SET_ORIGIN_PORT,
	GET_ORIGIN_PORT,
	SET_FILTER,
	SET_MGMT_ADDR,
	GET_MGMT_ADDR,
	SET_MGMT_PORT,
	GET_MGMT_PORT,
	SET_ORIGIN_ADDR
} RequestStatus;

void processCmd(const char *buffer, size_t len, int socket, struct sockaddr *clientAddr, socklen_t clientAddrLen,
                ServerArguments * args);
#endif
