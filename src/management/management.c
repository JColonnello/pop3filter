#include "management.h"
#include "../arguments/args.h"
#include "../functions.h"
#include "../parsers/management.h"
#include "../stats.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdlib.h>

void processCmd(const char *buffer, size_t len, int socket, struct sockaddr *clientAddr, socklen_t clientAddrLen,
                ServerArguments args)
{
	static char statsBuffer[256];
	char *errorFile;
	char *proxy_addr;
	char *data = NULL;
	size_t dataLen;
	RequestStatus status =
	    lexRequest(buffer, len, (const char **)&data, &dataLen); // TODO: se podria dejar en el buffer la data
	if (data != NULL)
		data[dataLen] = 0;

	switch (status)
	{
	case STATS: {
		size_t statsLen = stats(statsBuffer);
		sendto(socket, statsBuffer, statsLen, 0, clientAddr, clientAddrLen);
		break;
	}
	case SET_ERROR_FILE:
		set_error_file(args, data); // TODO: Obtener data
		break;
	case GET_ERROR_FILE:
		errorFile = get_error_file(args);
		break;
	case SET_PROXY_ADDR:
		// TODO: Obtener data
		inet_pton(AF_INET, data, &(args.listenSock.sin_addr));
		break;
	case GET_PROXY_ADDR:
		proxy_addr = inet_ntoa(args.listenSock.sin_addr);
		break;
	case SET_LISTEN_PORT:
		// TODO: Obtener data
		args.listenSock.sin_port = htons(atoi(data));
		break;
	case GET_LISTEN_PORT: {
		unsigned short port = args.listenSock.sin_port;
		break;
	}
	case SET_ORIGIN_PORT:
		args.originPort = data;
		break;
	case GET_ORIGIN_PORT: {
		char *port = args.originPort;
		break;
	}
	case SET_FILTER:
		args.filterCmd = data;
		break;
	case SET_MGMT_ADDR:
		inet_pton(AF_INET, data, &(args.mgmtSock.sin_addr));
		break;
	case GET_MGMT_ADDR:
		proxy_addr = inet_ntoa(args.listenSock.sin_addr);
		break;
	case SET_MGMT_PORT:
		args.mgmtSock.sin_port = htons(atoi(data));

		break;
	case GET_MGMT_PORT: {
		unsigned short port = args.mgmtSock.sin_port;
		break;
	}
	default:
		// TODO: informar comando incorrecto
		break;
	}
}

void receiveRequest(char *buffer, size_t len, int socket, struct sockaddr *clientAddr, socklen_t clientAddrLen,
                    ServerArguments args)
{
	// TODO: Crear socket y recibir las request de los usuarios
	processCmd(buffer, len, socket, clientAddr, clientAddrLen, args);
}
