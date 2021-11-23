#include "stats.h"
#include <stdio.h>

struct Stats _stats;

size_t stats(char *stats)
{
	return sprintf(stats,
	               "Cantidad de conexiones: %d\r\nCantidad de conexiones concurrentes: %d\r\nCantidad de bytes "
	               "transferidos:%d\r\n",
	               _stats.connections, _stats.current_connections, _stats.bytes_trans);
}

void addConnection()
{
	_stats.connections++;
}

void addCurrentConnection()
{
	_stats.current_connections++;
	addConnection();
}

void removeCurrentConnection(){
	_stats.current_connections--;
}

void addBytes(int bytes)
{
	_stats.bytes_trans += bytes;
}