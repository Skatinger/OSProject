#ifndef HEADER_FILE
#define HEADER_FILE

#define ALPHABET_SIZE 26
#define ERROR 1
#define SUCCESS 0

//KVS INTERFACE FUNCTIONS

typedef struct KVS KVS;


KVS* create(int size);

void destroy(KVS* store);

int set(KVS* store, char* key, char* value);

char* get(KVS* store, char* key);

char* del(KVS* store, char* key);

int replace(KVS* store, char* key, char* value);

float loadfactor (KVS *store);
#endif
