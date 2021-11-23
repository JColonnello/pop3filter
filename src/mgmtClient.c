#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT     8080
#define MAXLINE 1024

typedef enum
{
	ERROR,
	STATS,
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
	GET_MGMT_PORT
} RequestStatus;
  
typedef struct request
{
    RequestStatus status;
    char * data;
    size_t dataLen;
}Request;


int main (int argc, char * argv[]){
    
    int sockfd;
    struct sockaddr_in servaddr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(-1);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    Request req;
    memset(&req, 0, sizeof(req));


    int len;

	char msg[1024];
	size_t msgLen;
    

	int c;

	for(;;)
	{
		c = getopt(argc, argv, "hl:L:o:e:t:p:P:u:v");
		if (c == -1)
			break;

		switch (c)
		{
		case 'e':
            req.status = SET_ERROR_FILE;
            req.data = msg;
            req.dataLen = sprintf(req.data, optarg);
			break;
		case 'l':
            req.status = SET_PROXY_ADDR;
            req.data = msg;
            req.dataLen = sprintf(req.data, optarg);
			break;
		case 'L':
            req.status = SET_MGMT_ADDR;
            req.data = msg;
            req.dataLen = sprintf(req.data, optarg);
			break;
		/*
        case 't':
            req.status = SET_ERROR_FILE;
            req.data = msg;
            req.dataLen = sprintf(req.data, optarg);
			break;
		*/
        case 'p':
            req.status = SET_LISTEN_PORT;
            req.data = msg;
            req.dataLen = sprintf(req.data, optarg);
			break;
		case 'P':
            req.status = SET_ORIGIN_PORT;
            req.data = msg;
            req.dataLen = sprintf(req.data, optarg);
			break;
		case 'o':
            req.status = SET_MGMT_PORT;
            req.data = msg;
            req.dataLen = sprintf(req.data, optarg);
			break;
		default:
            req.status= ERROR;
            req.data = msg;
            req.dataLen = sprintf(msg, "Unknown command\n");
			exit(1);
		}
    sendto(sockfd, msg, msgLen, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));

	}


	return 0;

    
}