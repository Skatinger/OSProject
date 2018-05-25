#ifndef KV3_HEADER_FILE
#define KV3_HEADER_FILE

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define NO_SUCH_KEY 12
#define KEY_TAKEN 23
#define OVERFLOW 99
#define ERR 1

#define MAX_ADDRESS (unsigned int) 32768
#define SLOTS 512
#define ALPHABET_SIZE 37


///* =================================================================================================== */
///  STRUCT defs
///* =================================================================================================== */

/**
 * A pair of a key and a value.
 */
typedef struct key_value_pair {
    char* key;
    void* value;
} Kvp;

/**
 * A node in the hash circle, i.e. a virtual 'system', i.e. one of the possibly
 * multiple key value tables.
 * Stored in a linked list as defined below.
 */
typedef struct node {
    unsigned int hash_angle;
    Kvp* table[SLOTS];   // the actual content
    int load;
    struct node* next; // the next such node in the linked list
} Node;

/**
 * A linked list that will be used to hold the tables currently available.
 */
typedef struct linked_list {
    int nb_nodes;
    Node* first;
    Node* last;
} LL;

/**
 * The structure holding the key-value store.
 */
typedef struct KVS {
    LL* key_nodes;          // where the stuff hashed by key will e
    int load;
} KVS;

///* =================================================================================================== */
///  KVS INTERFACE FUNCTIONS -- PUBLIC
///* =================================================================================================== */



/**
 * creates a new Key-Value store
 * @return <c>struct KVS</c> pointer to a new Key-Value store
 */
KVS* create();

/**
 * destroys a Key-Value store
 * @param <c>store</c> the store to be destroyed
 */
void destroy(KVS* store);

/**
 * inserts a new key-value pair into a store
 * @param <c>struct KVS* store</c> the ks-store to insert the ks-pair
 * @param <c>char* key</c> key of the pair to be inserted
 * @param <c>void* value</> value of the pair to be inserted
 * @return <c>int</c> SUCCESS if everything's fine, one of the error codes if not
 */
int set(KVS* store, char* key, void* value);

/**
 * searches for a given key and its value
 * @param <c>struct KVS store</c> the store to be searched
 * @param <c>char* key</c> the key to be searched for
 * @return <c>void*</c> the found key or NULL
 */
void* get(KVS* store, char* key);

/**
 * deletes a key-value pair from a store.
 * This does not free the memory as otherwise the return value couldn't be accessed.
 * @param <c>struct KVS* store</c> the store from which to delete the key
 * @param <c>char* key</c> the key of the pair to be deleted
 * @return <c>void*</c> the deleted value
 */
void* del(KVS* store, char* key);

/**
 * replaces a value with another value. This does not change the key.
 * @param <c>struct KVS* store</c> the store in which to replace a ks-pair
 * @param <c>char* key</c> key of the pair to replace
 * @param <c>void* value</c> the value to be inserted in place of the previous
 * @return <c>int</c> SUCCESS on success, one of the error codes if not.
 */
int replace(KVS* store, char* key, void* value);

/**
 * calculates the loadfactor of the whole key-value store, which in praxis will never be more than 90%.
 * @param <c>struct KVS* store</c> the store of which the loadfactor should be calculated
 * @return <c>float</c> the loadfactor of the key-value store
 */
float avg_loadfactor (KVS *store);

/**
 * Incredibly inefficient way to retrieve the keys to a certain value, if the value is a string (char*).
 * The keys will be returned as a dash-separated list.
 * @param kvs the kvs to look in
 * @param value the value one wants the keys to
 * @return the keys that have the given value
 */
char *keys_for_string_value(KVS *kvs, char* value);

///* =================================================================================================== */
///  DEBUG FUNCTIONS
///* =================================================================================================== */

/**
 * Prints the hashtables in a linked list of hashtables (nodes) to stdout
 * @param list the list to print
 * @param print_kvps whether or not to print the keyvaluepairs in a node
 */
void print_tables_list(LL* list, int print_kvps);

/**
 * Print the content of a kvp table to stdout.
 * @param table the table to print
 */
void print_string_kvp_table(Kvp** table);

/**
 * Prints the whole keyvaluestore and info about it.
 * @param kvs
 */
void print_kvs(KVS* kvs);


///* =================================================================================================== */
///  AUXILIARY / 'private' FUNCTIONS
///* =================================================================================================== */

/**
 * Determines the position of a certain key on the circle using a hashfunction.
 * @param key the key to hash
 * @return the angle on the circle.
 */
unsigned int hash_angle(char* key);

/**
 * Creates a new LinkedList of Nodes.
 * @return pointer to the list.
 */
static LL* create_LL();

/**
 * Destroys a list of nodes.
 * @param list the list to get rid of
 */
static void destroy_LL(LL* list);

/**
 * Create a new node with given hash_angle.
 * Its size is a constant defined above.
 * @param hash_angle A number n s.th. 0 <= n <= MAX_ADRESS that determines the position of the node on the circle
 * @return pointer to the fresh new node.
 */
static Node* create_node(unsigned int hash_angle);

/**
 * Destroys a node and its contents.
 * @param node the node to get rid of.
 */
static void destroy_node(Node* node);

/**
 * Insert a new node in the sorted list of nodes.
 * @param list the list in which to put the node
 * @param node the new node.
 */
static void insert_node(LL* list, Node* node);

/**
 * Removes a node at a given angle from the given list.
 * @param list the list where the node should be removed
 * @param hash_angle the angle where the node to be removed is positioned
 */
static void remove_node(LL* list, unsigned int hash_angle);

/**
 * This function is used when a node becomes full (i.e.loadfactor > 0.9) and must be split up. This function takes
 * care of creating a new node between the full one and its closest predecessor, inserting it into the list and trans-
 * ferring all the kvp that now belong there.
 * While this is happening, nothing else should on the db.
 * @param list the node list
 * @param node the full node
 * @param prev the last node before the full one
 * @return
 */
static int split_up_node(LL* list, Node* node, Node* prev);

/**
 * This function is used when a node is almost empty and can be joined with its successor. It takes care of moving the kvp
 * and deleting the old node.
 * To be precise, if loadfactor(node) + loadfactor(node->next) < 0.7, they get joined.
 * @param list the node list
 * @param node the almost empty node
 */
static void merge_with_successor(LL* list, Node* node);

/**
 * This function is used when a node is searched for a new kvp. The first node on the circle which comes after it
 * will be returned.
 * @param list nodelist
 * @param hash_angle the angle after one wants a space
 * @return the node
 */
static Node* find_node_after(LL* list, unsigned int hash_angle);

/**
 * Returning the last node before a certain angle. This is mainly used to calulate the distance between a full node
 * and its last neighbour.
 * @param list nodelist
 * @param hash_angle the angle before which one wants to find a node
 * @return the node
 */
static Node* find_node_before_after(LL* list, unsigned int hash_angle);

/**
 * Hash function for hashing inside a node
 * @param key the key to hash
 * @return a hashvalue, 0 <= h <= SLOTS-1
 */
static int inside_node_hash(char* key);

/**
 * Rehashing function used for rehashing inside a node.
 * @param index the index of the first hashfunction.
 * @return a rehash, 1 < h
 */
static int inside_node_rehash(int index);

/**
 * Puts a kvp into a node (in a hashtable, if you will).
 * Returns an error if key was already in there.
 * @param node
 * @param kvp
 * @return 0 on success, error code otherwise
 */
static int insert_into_node(Node* node, Kvp* kvp);

/**
 * Gets the value for a certain key inside a node.
 * @param node the node to query
 * @param key
 * @return the value of NULL if not found
 */
static void* get_from_node(Node* node, char* key);

/**
 * Deletes a kvp with given key from the node.
 * @param node
 * @param key
 * @return as usual
 */
static void* remove_from_node(Node* node, char* key);

/**
 * Replaces the value of a key with a new value.
 * @param node the node where it should happen
 * @param key
 * @param new_valaue the new value for the given key
 * @return
 */
static int replace_in_node(Node* node, char* key, void* new_valaue);

/**
 * Gets all the keys that map to a certain value, provided that the value be of type char*!
 * @param node
 * @param value
 * @return
 */
static char* keys_for_string_value_in_node(Node* node, char* value);

/**
 * Returns the loadfactor of a node (i.e. of one of the hashtables)
 * @param node
 * @return
 */
static double loadfactor(Node* node);

/**
 * Initialises a new keyvaluepair.
 * @return pointer to the pair.
 */
static Kvp* init_kvp();

static Kvp* string_kvp(char* key, char* value);

/**
 * Frees a kvp.
 * @param kvp
 */
static void free_kvp(Kvp* kvp);
#endif
