#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <limits.h>    /* LONG_MIN et al */
#include <string.h>    /* memset */
#include <errno.h>
#include <getopt.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <arpa/inet.h>

#include "args.h"

#define DEFAULT_MGMT_PORT 9090
#define DEFAULT_LISTEN_PORT 1110
#define DEFAULT_ORIGIN_PORT "110"

static void
version(void) {
    fprintf(stderr, "POP3Filter 0.1\n"
                    "ITBA Protocolos de Comunicación 2021/2 -- Grupo 9\n"
                    );
}

static void
usage(const char *progname) {
    fprintf(stderr, "Escribir ayuda para el programa %s", progname);
    exit(1);
}

// con fprintf(stderr, "ERROR MESSAGE"); escribo el file
int error_file(ServerArguments args, char * file){
    args.logFile= file;
    printf("errores\n");
    int out = open(file, O_RDWR|O_CREAT|O_APPEND, 0600);
    if(out == -1) {
        perror(" error opening file");
        return -1;
    }
    int save_out = dup(fileno(stderr));
    if(-1 == dup2(out, fileno(stderr))) {
        perror("cannot redirect stderr");
        return -1;
    }    
}


void create_filter(char * cmd){
    //TODO: Mandar respuesta del server origin
    strcat(cmd, "RESPONSE");
    system(cmd);
}

void main(int argc, char **argv) {
    ServerArguments args;
    memset(&args, 0, sizeof(ServerArguments)); // sobre todo para setear en null los punteros de users

    /*      VALORES POR DEFECTO     */
    args.logFile = "/dev/null";
    inet_pton(AF_INET, "127.0.0.1", &(args.mgmtSock.sin_addr));
    args.mgmtSock.sin_port = htons(DEFAULT_MGMT_PORT);
//  Por defecto escucha en todas las interfaces   
//  inet_pton(AF_INET, "127.0.0.1", &(args.listenSock.sin_addr));
    args.listenSock.sin_port = htons(DEFAULT_LISTEN_PORT);
    args.originPort = DEFAULT_ORIGIN_PORT;
    args.originServer = "";        //TODO: de donde lo tomo?
    args.filterCmd = "";
    /********************************/
    
    int c;


    while (true) {
        c = getopt(argc, argv, "hl:L:o:e:t:p:P:u:v");
        if (c == -1)
            break;

        switch (c) {        
            case 'h':
                usage(argv[0]);
                break;
            case 'e':
                error_file(args, optarg);
                break;
            case 'l':
                inet_pton(AF_INET, optarg, &(args.listenSock.sin_addr));
                break;
            case 'L':
                inet_pton(AF_INET, optarg, &(args.mgmtSock.sin_addr));
                break;
            case 't':
                create_filter(optarg);
                //TODO: crear proceso que corra el comando que esta en optarg
                break;
            case 'p':
                args.listenSock.sin_port = htons(atoi(optarg));
                break;
            case 'P':
            args.originPort = optarg;
                break;
            case 'o':
                args.mgmtSock.sin_port = htons(atoi(optarg));
                break;
            case 'v':
                version();
                exit(0);
                break;
            default:
                fprintf(stderr, "unknown argument %d.\n", c);
                exit(1);
        }

    }


   
}

