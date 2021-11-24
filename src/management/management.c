#include "management.h"
#include "../arguments/args.h"
#include "../enviroment.h"
#include "../functions.h"
#include "../parsers/management.h"
#include "../stats.h"
#include "logger.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdlib.h>

#define ERROR_MSG "Unknown cmd \n"
void processCmd(const char *buffer, size_t len, int socket, struct sockaddr *clientAddr, socklen_t clientAddrLen,
                ServerArguments *args)
{
	char msg[256] = {'\0'};
	size_t msgLen = 0;
	// char *errorFile;
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
		break;
	}
	case SET_ORIGIN_ADDR:
		msgLen = set_origin_addr(args, data, msg);
		break;
	case GET_ERROR_FILE:
		log(LOG_DEBUG, "Log file is: %s", args->logFile);
		msgLen = get_error_file(args, msg);
		break;

	case GET_PROXY_ADDR:
		log(LOG_DEBUG, "Getting proxy addr");
		msgLen = get_proxy_addr(args, msg);
		break;
	case GET_LISTEN_PORT: {
		log(LOG_DEBUG, "Getting listen port");
		msgLen = get_listen_port(args, msg);
		break;
	}
	case GET_ORIGIN_PORT: {
		log(LOG_DEBUG, "Getting origin port");
		msgLen = get_origin_port(args, msg);
		break;
	}
	case GET_MGMT_ADDR:
		log(LOG_DEBUG, "Getting management addr");
		msgLen = get_mgmt_addr(args, msg);
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
		msgLen = get_error(msg);
		break;
	default:
		// TODO: informar comando incorrecto
		break;
	}
	//TODO: IMPRIMIR INFO CLIENTE
	ssize_t nbs = sendto(socket, msg, msgLen, 0, clientAddr, clientAddrLen);
	if (nbs == -1)
	{
		log(LOG_ERROR, "Failed to send mgmt response");
		return;
	}
	log(LOG_DEBUG, "Counting bytes");
	addBytes(nbs);
	memset(msg, '\0', sizeof(msg));
	msgLen = 0;
}
/*
void receiveRequest(char *buffer, size_t len, int socket, struct sockaddr *clientAddr, socklen_t clientAddrLen,
                    ServerArguments * args)
{
	// TODO: Crear socket y recibir las request de los usuarios
	processCmd(buffer, len, socket, clientAddr, clientAddrLen, args);
}
*/