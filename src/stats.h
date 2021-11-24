#include <stddef.h>

struct Stats
{
	unsigned connections;
	unsigned current_connections;
	unsigned bytes_trans;
	// TODO: considerar agregar alguna metrica mas
};

size_t stats(char *stats);

void addCurrentConnection();
void removeCurrentConnection();
void addBytes(int bytes);