struct Stats {
    unsigned connections;
    unsigned current_connections;
    unsigned bytes_trans;
    //TODO: considerar agregar alguna metrica mas

}

size_t stats(char * stats);

void addConnection();
void addCurrentConnection();
void addBytes(int bytes);