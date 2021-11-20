/*
https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168
*/

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<stdlib.h>    //malloc
#include<sys/socket.h>    //you know what this is for
#include<arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include<netinet/in.h>
#include<unistd.h>    //getpid
 
#include "utils.h"




int main( int argc , char *argv[])
{
    unsigned char hostname[100];
 
    //Get the DNS servers from the resolv.conf file
    get_dns_servers();
     
    //Get the hostname from the terminal
    printf("Enter Hostname to Lookup : ");
    scanf("%s" , hostname);
     
    //Now get the ip of this hostname , A record
    ngethostbyname(hostname , T_A);
 
    return 0;
}
 