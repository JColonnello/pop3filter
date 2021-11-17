#include "functions.h"


// con fprintf(stderr, "ERROR MESSAGE"); escribo el file
int set_error_file(ServerArguments args, char * file){
    args.logFile= file;
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

char * get_error_file(ServerArguments args) {
    return args.logFile;
}