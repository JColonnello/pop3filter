#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT     9090
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
    socklen_t servAddrLen = sizeof(servaddr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(-1);
    }
    memset(&servaddr, 0, sizeof(servaddr));


    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    char message[2048];
    memset(message, '\0', sizeof(message));
    char ans[2048];
    memset(ans, '\0', sizeof(message));
    while(fgets(message, sizeof(message), stdin) != NULL)
    {
        if(sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
            printf("ERROR MANDANDO INFO");
        }
        if(recvfrom(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&servaddr, &servAddrLen) < 0){
            printf("Error while receiving server's msg\n");
            return -1;
        }
    }   
     
   /*fgets(message, sizeof(message), stdin);
   sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    */

    printf("Server's response: %s\n", ans);
   
    
    return 0;
}
