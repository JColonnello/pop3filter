#include <errno.h>
#include <getopt.h>
#include <limits.h> /* LONG_MIN et al */
#include <stdio.h>  /* for printf */
#include <stdlib.h> /* for exit */
#include <string.h> /* memset */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "../functions.h"

#include <arpa/inet.h>

#include "args.h"

#define DEFAULT_MGMT_PORT "9090"
#define DEFAULT_LISTEN_PORT "1110"
#define DEFAULT_ORIGIN_PORT "110"

ServerArguments parseArguments(int argc, char *argv[])
{
	ServerArguments args;
	char msg[1024];
	size_t msgLen;
	memset(&args, 0, sizeof(ServerArguments)); // sobre todo para setear en null los punteros de users

	/*      VALORES POR DEFECTO     */
	set_error_file(&args, "/dev/null", msg);
	set_mgmt_addr(&args, "127.0.0.1", msg);
	set_mgmt_port(&args, DEFAULT_MGMT_PORT, msg);
		//  Por defecto escucha en todas las interfaces
	//  inet_pton(AF_INET, "127.0.0.1", &(args.listenSock.sin_addr));
	set_listen_port(&args, DEFAULT_LISTEN_PORT, msg);
	set_origin_port(&args, DEFAULT_ORIGIN_PORT, msg);

	args.originServer = ""; // TODO: de donde lo tomo?
	args.filterCmd = "";
	/********************************/

	int c;

	while (true)
	{
		c = getopt(argc, argv, "hl:L:o:e:t:p:P:u:v");
		if (c == -1)
			break;

		switch (c)
		{
		case 'h':
			msgLen = get_help(msg);
			printf(msg);
			exit(0);
			break;
		case 'e':
			msgLen = set_error_file(&args, optarg, msg);
			break;
		case 'l':
            msgLen = set_proxy_addr(&args, optarg, msg);
			break;
		case 'L':
            msgLen = set_mgmt_addr(&args, optarg, msg);
			break;
		case 't':
            set_filter(&args, optarg, msg);
			// TODO: crear proceso que corra el comando que esta en optarg
			break;
		case 'p':
            msgLen = set_listen_port(&args, optarg, msg);
			break;
		case 'P':
            msgLen = set_origin_port(&args, optarg, msg);
			break;
		case 'o':
            msgLen = set_mgmt_port(&args, optarg, msg);
			break;
		case 'v':
			msgLen = get_version(msg);
			printf(msg);
			exit(0);
			break;
		default:
            msgLen = get_error(msg);
			printf(msg);
			exit(1);
		}
	}

	return args;
}
