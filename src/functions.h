#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "./arguments/args.h"

size_t set_error_file(ServerArguments *args, char *file, char *msg);
size_t get_error_file(ServerArguments *args, char *msg);
size_t set_proxy_addr(ServerArguments *args, char *addr, char *msg);
size_t get_proxy_addr(ServerArguments *args, char *msg);
size_t set_listen_port(ServerArguments *args, char *port, char *msg);
size_t get_listen_port(ServerArguments *args, char *msg);
size_t set_origin_port(ServerArguments *args, char *port, char *msg);
size_t get_origin_port(ServerArguments *args, char *msg);
size_t set_origin_addr(ServerArguments *args, char *addr, char *msg);
int set_filter(ServerArguments *args, char *username, int *fd1);
size_t set_mgmt_addr(ServerArguments *args, char *addr, char *msg);
size_t get_mgmt_addr(ServerArguments *args, char *msg);
size_t set_mgmt_port(ServerArguments *args, char *port, char *msg);
size_t get_mgmt_port(ServerArguments *args, char *msg);
size_t get_error(char *msg);
bool create_child(ServerArguments args, char *name);
size_t get_help(char *msg);
size_t get_version(char *msg);

#endif
