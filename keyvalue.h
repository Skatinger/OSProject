#ifndef HEADER_FILE
#define HEADER_FILE

//DEFS

//VARS
struct KVS {
    
    
    
} KVS;


//FUNCTIONS


KVS kvs_create();

void kvs_destroy(KVS* store);

int kvs_put_value(KVS* store, char* value);

char* kvs_get_value(KVS* store, char* key);

char* kvs_delete(KVS* store, char* key);

char* kvs_replace(KVS* store, char* key);
#endif
