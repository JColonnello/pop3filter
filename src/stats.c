#include "stats.h"

struct Stats stats;

size_t stats(char * stats){
    return sprintf(stats, "Cantidad de conexiones: %d\r\nCantidad de conexiones concurrentes: %d\r\n Cantidad de bytes transferidos:%d\r\n", stats.connections, stats.current_connection, stats.bytes_trans);
}

void addConnection(){
    stats.connections++;
}

void addCurrentConnection(){
    stats.current_connection++;
    addConection();
}

void addBytes(int bytes){
    stats.bytes_trans += bytes;
}