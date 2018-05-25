#ifndef HEADER_FILE
#define HEADER_FILE

#include "../project.h"

#define ALPHABET_SIZE 26
//#define ERROR 0
//#define SUCCESS 1
#define SUCCESS 0
#define STORAGE_FULL_ERROR 1
#define KEY_NOT_FOUND_ERROR 2
#define KEY_IN_USE_ERROR 3
#define STORAGE_EMPTY_ERROR "Deleting failed -- storage is empty\n"
#define KEY_NOT_FOUND_ERRORmsg "The desired key wasn't found\n"

#define BIGPRIME_1 1300391
#define BIGPRIME_2 224443

/* ==== structs =====*/
typedef struct keyValuePair {
  char* key;
  void* value;
} keyValuePair;

typedef struct KVS {
    int size;
    int load;
    keyValuePair *table;

} KVS;
/* ==================== */

//KVS INTERFACE FUNCTIONS

/**
 * a pair of key and value used to store values in a key-value store
 * @member <c>char* key</c> the key
 * @member <c>char* value</c> the value saved to its key
 * @member <c>struct keyValuePair *next</c> a pointer to the next element
 */
typedef struct keyValuePair keyValuePair;

/**
 * Key-Value store
 * @member <c>int size</c> the capacity of the key-value store
 * @member <c>int load</c> the used space of the key-value store
 * @member <c>struct keyValuePair</c> the table of key-value pairs in the store
 */
typedef struct KVS KVS;

/**
 * creates a new Key-Value store
 * @param <c>int size</c> desired initial size of the store in number of key-value pairs
 * @return <c>struct KVS</c> a new Key-Value store
 */
KVS* kvs_create(int size);

/**
 * destroys a Key-Value store
 * @param <c>store</c> the store to be destroyed
 */
void kvs_destroy(KVS* store);

/**
 * inserts a new key-value pair into a store
 * @param <c>struct KVS* store</c> the ks-store to insert the ks-pair
 * @param <c>char* key</c> key of the pair to be inserted
 * @param <c>char* value</> value of the pair to be inserted
 * @return <c>int</c> 0 if error occured, 1 otherwise
 */
int kvs_set(KVS* store, char* key, void* value);

/**
 * searches for a given key and its value
 * @param <c>struct KVS store</c> the store to be searched
 * @param <c>char* key</c> the key to be searched for
 * @return <c>char*</c> the found key or an empty string
 */
void* kvs_get(KVS* store, char* key);

/**
 * deletes a key-value pair from a store
 * @param <c>struct KVS* store</c> the store from which to delete the key
 * @param <c>char* key</c> the key of the pair to be deleted
 * @return <c>char*</c> the deleted value
 */
void* kvs_del(KVS* store, char* key);

/**
 * replaces a value with another value. This does change the key as well
 * @param <c>struct KVS* store</c> the store in which to replace a ks-pair
 * @param <c>char* key</c> key of the pair to replace
 * @param <c>char* value</c> the value to be inserted in place of the previous
 * @return <c>int</c> 0 if error occured, 1 otherwise
 */
int kvs_replace(KVS* store, char* key, void* value);

/**
 * calculates the load of a key-value store
 * @param <c>struct KVS* store</c> the store of which the loadfactor should be calculated
 * @return <c>float</c> the loadfactor of the key-value store
 */
float loadfactor (KVS *store);

void printKVS(KVS* store);

#endif


/* General comments, annotations */

    /**
     *
     */
