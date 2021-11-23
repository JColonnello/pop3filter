#include "management.h"
#include "../arguments/args.h"
#include "../functions.h"
#include "../parsers/management.h"
#include "../stats.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdlib.h>
#include "logger.h"

#define ERROR_MSG "Unknown cmd \n"
void processCmd(const char *buffer, size_t len, int socket, struct sockaddr *clientAddr, socklen_t clientAddrLen,
                ServerArguments args)
{
	char msg[256];
	size_t msgLen;
	char *errorFile;
	char *data = NULL;
	size_t dataLen;
	RequestStatus status =
	    lexRequest(buffer, len, (const char **)&data, &dataLen); // TODO: se podria dejar en el buffer la data
	if (data != NULL)
		data[dataLen] = 0;

	switch (status)
	{
	case STATS: {
		log(LOG_DEBUG, "Getting stats");
		msgLen = stats(msg);
		sendto(socket, msg, msg, 0, clientAddr, clientAddrLen);
		break;
	}
	case SET_ERROR_FILE:
		log(LOG_DEBUG, "New log file: %s", data);
		msgLen = set_error_file(args, data, msg);
		break;
	case GET_ERROR_FILE:
		log(LOG_DEBUG, "Log file is: %s", args.logFile);
		msgLen = get_error_file(args, msg);
		break;
	case SET_PROXY_ADDR:
		log(LOG_DEBUG, "New proxy addr: %s", data);
		msgLen = set_proxy_addr(args, data, msg);
		break;
	case GET_PROXY_ADDR:
		log(LOG_DEBUG, "Getting proxy addr");
		msgLen = get_proxy_addr(args, msg);
		break;
	case SET_LISTEN_PORT:
		log(LOG_DEBUG, "New listen port is: %s", data);
		msgLen = set_listen_port(args, data, msg);
		break;
	case GET_LISTEN_PORT: {
		log(LOG_DEBUG, "Getting listen port");
		msgLen = get_listen_port(args, msg);
		break;
	}
	case SET_ORIGIN_PORT:
		log(LOG_DEBUG, "New origin port is: %s", data);
		msgLen = set_origin_port(args, data, msg);
		break;
	case GET_ORIGIN_PORT: {
		log(LOG_DEBUG, "Getting origin port");
		msgLen = get_origin_port(args, msg);
		break;
	}
	case SET_FILTER:
		log(LOG_DEBUG, "New filter is: %s", data);
		msgLen = set_filter(args, data, msg);
		break;
	case SET_MGMT_ADDR:
		log(LOG_DEBUG, "New management addr is: %s", data);
		msgLen = set_mgmt_addr(args, data, msg);
		break;
	case GET_MGMT_ADDR:
		log(LOG_DEBUG, "Getting management addr");
		msgLen = get_mgmt_addr(args, msg);
		break;
	case SET_MGMT_PORT:
		log(LOG_DEBUG, "New management port is: %s", data);
		msgLen = set_mgmt_port(args, data, msg);
		break;
/*
	case GET_MGMT_PORT: {		//TODO: este caso no existe
		log(LOG_DEBUG, "Getting management port");
		msgLen = get_mgmt_port(args, msg);
		break;
	}
*/
	case ERROR:
		log(LOG_DEBUG, "Sending error msg");
		msgLen = set_error(msg);
		break;
	default:
		// TODO: informar comando incorrecto
		break;
	}
	sendto(socket, msg, msgLen, 0, clientAddr, clientAddrLen);

}

void receiveRequest(char *buffer, size_t len, int socket, struct sockaddr *clientAddr, socklen_t clientAddrLen,
                    ServerArguments args)
{
	// TODO: Crear socket y recibir las request de los usuarios
	processCmd(buffer, len, socket, clientAddr, clientAddrLen, args);
}