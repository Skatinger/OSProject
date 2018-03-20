#ifndef HEADER_FILE
#define HEADER_FILE

#define ALPHABET_SIZE 26

//KVS INTERFACE FUNCTIONS

typedef struct KVS KVS;


KVS kvs_create();

void destroy(KVS* store);

void set(KVS* store, char* value, char* key);

char* get(KVS* store, char* key);

char* del(KVS* store, char* key);

void replace(KVS* store, char* key);

float loadfactor (KVS *store);
#endif
