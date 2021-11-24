#include <errno.h>
#include <getopt.h>
#include <limits.h> /* LONG_MIN et al */
#include <stdio.h>  /* for printf */
#include <stdlib.h> /* for exit */
#include <string.h> /* memset */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "../logger.h"

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
	memset(&args, 0, sizeof(ServerArguments)); // sobre todo para setear en null los punteros de users

	if (argc <= 1)
	{
		fprintf(stderr, "Insuficientes argumentos\n");
		exit(1);
	}

	/*      VALORES POR DEFECTO     */
	set_error_file(&args, "/dev/null", msg);
	set_mgmt_addr(&args, "127.0.0.1", msg);
	set_mgmt_port(&args, DEFAULT_MGMT_PORT, msg);
	set_listen_port(&args, DEFAULT_LISTEN_PORT, msg);
	set_origin_port(&args, DEFAULT_ORIGIN_PORT, msg);

	args.originServer = ""; // TODO: de donde lo tomo?
	args.filterCmd = NULL;
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
			get_help(msg);
			printf("%s", msg);
			exit(0);
			break;
		case 'e':
			set_error_file(&args, optarg, msg);
			break;
		case 'l':
			set_proxy_addr(&args, optarg, msg);
			break;
		case 'L':
			set_mgmt_addr(&args, optarg, msg);
			break;
		case 't':
			args.filterCmd = optarg;
			break;
		case 'p':
			set_listen_port(&args, optarg, msg);
			break;
		case 'P':
			set_origin_port(&args, optarg, msg);
			break;
		case 'o':
			set_mgmt_port(&args, optarg, msg);
			break;
		case 'v':
			get_version(msg);
			printf("%s", msg);
			exit(0);
			break;
		default:
			get_error(msg);
			printf("%s", msg);
			exit(1);
		}
	}
	set_origin_addr(&args, argv[argc - 1], msg);
	return args;
}
