#ifndef KV2_HEADER_FILE
#define KV2_HEADER_FILE

#include "../project.h"

#define SUCCESS 0
#define ERR 1

/* This is yet to be optimised */
#define MAX_ADDRESS (unsigned int) 4294967295 // 2^32 - 1
#define SLOTS 16


/* ==== structs =====*/
/**
 * A pair of a key and a value.
 */
typedef struct key_value_pair {
  char* key;
  void* value;
} Kvp;

/**
 * A node in the hash circle, i.e. a virtual 'system', i.e. one of the possibly
 * mutliple key value tables.
 * Stored in a binary tree
 */
typedef struct node {
  unsigned int hash_angle;
  Kvp* table;   // the actual content
  int load;
  struct node* left;         // neighbours
  struct node* right;
} Node;

/**
 * A binary tree that will be used to hold the tables currently available.
 */
typedef struct binary_tree {
  int nbNodes;
  Node* root;
} BT;

/**
 *
 */
typedef struct KVS {
  BT* key_nodes;          // where the stuff hashed by key will e
  //BT* value_nodes;
  // size_t key_size;
  // size_t value_size;
  int load;
} KVS;
/* ==================== */

//KVS INTERFACE FUNCTIONS

/**
 * creates a new Key-Value store
 * @param <c>int size</c> desired initial size of the store in number of key-value pairs
 * @return <c>struct KVS</c> pointer to a new Key-Value store
 */
KVS* create(int size);

/**
 * destroys a Key-Value store
 * @param <c>store</c> the store to be destroyed
 */
void destroy(KVS* store);

/**
 * inserts a new key-value pair into a store
 * @param <c>struct KVS* store</c> the ks-store to insert the ks-pair
 * @param <c>char* key</c> key of the pair to be inserted
 * @param <c>char* value</> value of the pair to be inserted
 * @return <c>int</c> 0 if error occured, 1 otherwise
 */
int set(KVS* store, char* key, void* value);

/**
 * searches for a given key and its value
 * @param <c>struct KVS store</c> the store to be searched
 * @param <c>char* key</c> the key to be searched for
 * @return <c>char*</c> the found key or an empty string
 */
void* get(KVS* store, char* key);

/**
 * deletes a key-value pair from a store
 * @param <c>struct KVS* store</c> the store from which to delete the key
 * @param <c>char* key</c> the key of the pair to be deleted
 * @return <c>char*</c> the deleted value
 */
void* del(KVS* store, char* key);

/**
 * replaces a value with another value. This does change the key as well
 * @param <c>struct KVS* store</c> the store in which to replace a ks-pair
 * @param <c>char* key</c> key of the pair to replace
 * @param <c>char* value</c> the value to be inserted in place of the previous
 * @return <c>int</c> 0 if error occured, 1 otherwise
 */
int replace(KVS* store, char* key, void* value);

/**
 * calculates the load of a key-value store
 * @param <c>struct KVS* store</c> the store of which the loadfactor should be calculated
 * @return <c>float</c> the loadfactor of the key-value store
 */
float loadfactor (KVS *store);

/// DEBUG STUFF
void print_tables_tree(BT* tree);

// recursive thing to print everythin
void print_nodes(Node* node);

///////// AUXILIARY FUNCTIONS
static BT* create_BT();

static Node* create_node(unsigned int hash_angle);

static void destroy_node(Node* node);

static void insert_node(BT* tree, Node* node);

// non recursive
static void remove_node(BT* tree, unsigned int hash_angle);

static Node* find_node_after(BT* tree, unsigned int hash_angle);

static void insert_into_node(Node* node, Kvp kvp);
static Kvp init_kvp();

void printKVS(KVS* store);

#endif


/* General comments, annotations */

    /**
     *
     */
