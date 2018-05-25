#include "key_value_v3.h"
#include "../utils/logger.h"
#include "../utils/string_stuff.h"
#include <stdlib.h>
#include <string.h>



KVS* create() {
    KVS* kvs = malloc(sizeof(KVS));
    kvs->load = 0;
    kvs->key_nodes = create_LL();
    return kvs;
}

void destroy(KVS* kvs) {
    destroy_LL(kvs->key_nodes);
    free(kvs);
    kvs = NULL;
}

int set(KVS* kvs, char* key, void* value) {
    unsigned int angle = hash_angle(key);

    // we'll need the node before the one where we want to insert in case we need to split this node
    Node* before_dest = find_node_before_after(kvs->key_nodes, angle);
    Node* dest = before_dest->next;

    // in case the successor of that is null, it is the end of the list and we rebegin at the beginning since this is a circle
    if (dest == NULL) dest = kvs->key_nodes->first;

    int counter = 0;
    while (loadfactor(dest) > 0.9) {
        // create the new node and add it to the kvs
        split_up_node(kvs->key_nodes, dest, before_dest);

        // now let's check again where to put the new kvp
        before_dest = find_node_before_after(kvs->key_nodes, angle);
        dest = before_dest->next;
        if (dest == NULL) dest = kvs->key_nodes->first;

        // this is for security to make sure there's no infinite loop
        counter++;
        if (counter > MAX_ADDRESS) return OVERFLOW;
    }

    Kvp* new = init_kvp();
    new->key = key;
    new->value = value;

    kvs->load++;

    // return any error messages there might be
    return insert_into_node(dest, new);
}

void* get(KVS* kvs, char* key) {
    unsigned int angle = hash_angle(key);
    Node* node = find_node_after(kvs->key_nodes, angle);

    return get_from_node(node, key);
}

void* del(KVS* kvs, char* key) {
    unsigned int angle = hash_angle(key);
    Node* node = find_node_after(kvs->key_nodes, angle);
    Node* next;
    if (node->next != NULL ) next = node->next;
    else next = kvs->key_nodes->first;

    double factor = loadfactor(node);
    double factor_next = loadfactor(next);
    if (factor + factor_next < 0.7) {
       merge_with_successor(kvs->key_nodes, node);
        return remove_from_node(next, key);
    }

    kvs->load--;
    return remove_from_node(node, key);
}

int replace(KVS* kvs, char* key, void* value) {
    unsigned int angle = hash_angle(key);
    Node *node = find_node_after(kvs->key_nodes, angle);

    return replace_in_node(node, key, value);
}

float avg_loadfactor(KVS* kvs) {
    return (float) kvs->load / (float) (SLOTS*MAX_ADDRESS);
}

char* keys_for_string_value(KVS* kvs, char* value) {
    char* keys = "";
    char* k_for_v;
    for (Node *i = kvs->key_nodes->first; i != NULL; i = i->next) {
        k_for_v = keys_for_string_value_in_node(i, value);
        if (!strcmp(k_for_v, "")) {
            // dont add an empty string
            continue;
        } else if (!strcmp(keys, "")) {
            // if this is the first, dont add a ; before
            keys = k_for_v;
        } else {
            keys = concat(3, keys, "- ", k_for_v);
        }
    }
    return keys;
}

unsigned int hash_angle(char* key) {
    // TODO: Find good hash function THAT UNIFORMLY DISTRIBUTES STUFF ON CIRCLE
    unsigned int h;
    char* tmp = key;
    for (h = 0; *key != '\0'; key++) {
        h = (ALPHABET_SIZE * h + *key) % MAX_ADDRESS;
    }
    return h;
}

void print_kvs(KVS* kvs) {
    print_tables_list(kvs->key_nodes, TRUE);
}

static LL* create_LL() {
    LL *list = malloc(sizeof(LL));
    list->nb_nodes = 1;
    Node *first = create_node(MAX_ADDRESS);
    list->first = first;
    list->last = first;
    return list;
}

static int split_up_node(LL* list, Node *node, Node* prev) {
    // the new node should be inserted between this one and the one before
    // so the new one can 'consume' half of this one's kvps
    int new_angle = 0;
    if (prev->hash_angle < node->hash_angle) {
        new_angle = prev->hash_angle + ((node->hash_angle - prev->hash_angle) / 2);
    } else if (prev->hash_angle > node->hash_angle) {
        new_angle = prev->hash_angle + ((MAX_ADDRESS - prev->hash_angle + node->hash_angle) / 2);

    } else if (list->nb_nodes == 1) {
        new_angle = (node->hash_angle + MAX_ADDRESS / 2) % MAX_ADDRESS;
    }
    new_angle = new_angle % MAX_ADDRESS;

    Node *new = create_node(new_angle);
    insert_node(list, new);

    int kvs_angle;
    // check if the new node is 'before' (in the sense has a lower angle) or
    // 'after' on the circle

    if (new_angle < node->hash_angle) {
        // in that case, we remove all the kvp's from the node which have an
        // angle <= new angle
        // now see which elements need to be transferred to the new one
        for (int i = 0; i < SLOTS; i++) {
            if (node->table[i] == NULL) continue;
            kvs_angle = hash_angle(node->table[i]->key);
            if (kvs_angle <= new_angle) {
                insert_into_node(new, node->table[i]);
                node->table[i] = NULL;
                node->load--;
            }
            // otherwise leave it there
        }
    } else {
        // so the elements that have an angle <= new_angle need to be transferred
        for (int i = 0; i < SLOTS; i++) {
            if (node->table[i] == NULL) continue;
            kvs_angle = hash_angle(node->table[i]->key);
            if (kvs_angle > node->hash_angle) {
                insert_into_node(new, node->table[i]);
                node->table[i] = NULL;
                node->load--;
            }
            // otherwise leave it there
        }
    }
    return 0;
}

static void merge_with_successor(LL* list, Node* node) {
    if (node->hash_angle == MAX_ADDRESS) return;
    Node* next = node->next == NULL ? list->first : node->next;
    for (int i = 0; i < SLOTS; i++) {
        if (node->table[i] == NULL) continue;
        insert_into_node(next, node->table[i]);

    }
    remove_node(list, node->hash_angle);
}

static Node* create_node(unsigned int hash_angle) {
    Node* node = malloc(sizeof(Node));
    node->hash_angle = hash_angle;
    //node->table = malloc(SLOTS * sizeof(Kvp));
    for (int i = 0; i < SLOTS; i++) {
        node->table[i] = NULL;
    }
    node->next = NULL;
    node->load = 0;

    return node;
}

static void destroy_node(Node *node) {
    for (int i = 0; i < SLOTS; i++) {
        free_kvp(node->table[i]);
    }

    free(node);
    node = NULL;
}

static void destroy_node_leave_kvps(Node* node) {
    free(node);
}

static void insert_node(LL* list, Node *node) {
    Node *current = list->first;
    Node *previous = NULL;
    while (current != NULL && current->hash_angle < node->hash_angle) {
        previous = current;
        current = current->next;
    }

    // no duplicates
    if ((previous != NULL && previous->hash_angle == node->hash_angle)
        || (current != NULL && current->hash_angle == node->hash_angle)) {
        return;
    }

    if (previous == NULL) {
        // this goes into the first position of the list
        list->first = node;
    } else {
        previous->next = node;
        if (list->last == previous) list->last = node;
    }

    node->next = current;
    list->nb_nodes++;
}

// non recursive
static void remove_node(LL* list, unsigned int hash_angle) {
    if (list->nb_nodes < 2 || hash_angle == MAX_ADDRESS) return;

    Node* previous = find_node_before_after(list, hash_angle);
    Node* to_delete = previous->next == NULL ? list->first : previous->next;
    if (to_delete->hash_angle != hash_angle) /*nothing to delete then*/ return;


    previous->next = to_delete->next;
    if (list->last == to_delete) list->last = previous;
    if (list->first == to_delete) list->first = to_delete->next;


    destroy_node_leave_kvps(to_delete);
    list->nb_nodes--;
}

static Node* find_node_before_after(LL* list, unsigned int hash_angle) {
    Node *current = list->first;
    //Node* previous = NULL;
    while (current->next != NULL && current->next->hash_angle < hash_angle) {
        //previous = current;
        current = current->next;
    }
    //if (current != NULL) printf("current %d\n", current->hash_angle);
    //if (previous != NULL) printf("previous %d", previous->hash_angle);

    if (current == NULL) {
        // we went all the way to the end of the list, so take the beginning
        current = list->first;
    } else if (current == list->first && list->first->hash_angle >= hash_angle) {
        // that means the previous is the end of the list
        current = list->last;
    }
    return current;
}

static Node* find_node_after(LL* list, unsigned int hash_angle) {
    Node *node = find_node_before_after(list, hash_angle);
    if (node == NULL || node->next == NULL) return list->first;
    return node->next;
}

static int inside_node_hash(char* key) {
    int h;
    for (h = 0; *key != '\0'; key++) {
        h = (ALPHABET_SIZE * h + *key) % SLOTS;
    }
    return h % SLOTS;
}

static int inside_node_rehash(int index) {
    int ret = 1;
    int val = index;
    while (val > 0) {
        ret *= val % 10;
        val--;
    }
    return ret + 1;
}

static int insert_into_node(Node* node, Kvp* kvp) {
    // Return error if key is already taken
    if (get_from_node(node, kvp->key) != NULL) {
        return KEY_TAKEN;
    }
    int index = inside_node_hash(kvp->key);

    // rehash at most once
    if (node->table[index] != NULL) {
        index += inside_node_rehash(index);
        index = index % SLOTS;
    }

    // and from here try linearly
    // this cannot be infinite, as a node can never have a loadfactor of over 90%.
    while (node->table[index] != NULL) {
        index++;
        index = index % SLOTS;
    }
    node->table[index] = kvp;
    node->load++;
    return SUCCESS;
}

static void* remove_from_node(Node* node, char* key) {
    int index = inside_node_hash(key);

    // rehash once
    if (node->table[index] == NULL || strcmp(node->table[index]->key, key)) {
        index += inside_node_rehash(index);
        index = index % SLOTS;
    }
    // and from here try linearly
    int counter = 0;
    while (node->table[index] == NULL || strcmp(node->table[index]->key, key)) {
        // this is to ensure that this loop isn't infinite
        if (counter == SLOTS) return NULL;
        index++;
        counter++;
        index = index % SLOTS;

    }
    void *ret = node->table[index]->value;
    free_kvp(node->table[index]);
    node->table[index] = NULL;

    node->load--;

    return ret;
}

static int replace_in_node(Node* node, char* key, void* new_valaue) {
    int index = inside_node_hash(key);

    // rehash once
    if (node->table[index] != NULL) {
        index += inside_node_rehash(index);
        index = index % SLOTS;
    }
    // and from here try linearly
    int counter = 0;
    while (node->table[index] == NULL || strcmp(node->table[index]->key, key)) {
        // this is to ensure that this loop isn't infinite
        if (counter == SLOTS) return NO_SUCH_KEY;
        index++;
        counter++;
        index = index % SLOTS;
    }
    node->table[index]->value = new_valaue;

    return SUCCESS;
}

static void* get_from_node(Node* node, char* key) {
    int index = inside_node_hash(key);

    // rehash once
    if (node->table[index] != NULL) {
        index += inside_node_rehash(index);
        index = index % SLOTS;
    }
    // and from here try linearly
    int counter = 0;
    while (node->table[index] == NULL || strcmp(node->table[index]->key, key)) {
        // this is to ensure that this loop isn't infinite
        if (counter == SLOTS) return NULL;
        index++;
        counter++;
        index = index % SLOTS;
    }

    return node->table[index]->value;
}

static char* keys_for_string_value_in_node(Node* node, char* value) {
    char* keys = "";
    for (int i = 0; i < SLOTS; i++) {
        if (node->table[i] == NULL || node->table[i]->key == NULL) continue;
        if (!strcmp(value, (char *) node->table[i]->value)) {
            // if this is the first time
            if (!strcmp(keys, "")) {
                keys = node->table[i]->key;
            } else {
                keys = concat(3, keys, "- ", node->table[i]->key);
            }

        }
    }
    return keys;
}

static Kvp* init_kvp() {
    Kvp *kvp = malloc(sizeof(Kvp));
    kvp->key = NULL;
    kvp->value = NULL;
    return kvp;
}

static void free_kvp(Kvp* kvp) {
    if (kvp == NULL) return;
    //free(kvp->key);
    kvp->key = NULL;
    //free(kvp->value);
    kvp->value = NULL;
    free(kvp);
    kvp = NULL;
}

static void destroy_LL(LL* list) {
    Node* current = list->first;
    Node* prev = NULL;
    while (current != NULL) {
        prev = current;
        current = current->next;
        destroy_node(prev);
    }
    free(list);
    list = NULL;
}

void print_tables_list(LL* list, int print_kvps) {
    printf("Printing list. nb_nodes: %d, angle of first %d, angle of last %d\n", list->nb_nodes, list->first->hash_angle,
           list->last->hash_angle);
    Node* current = list->first;
    while (current != NULL) {
        printf("Node: \n");
        printf("\t-Angle: %u\n", current->hash_angle);
        if (print_kvps) {
            printf("\t-Table:\n");
            print_string_kvp_table(current->table);
        }
        printf("\t-Load: %d\n\n", current->load);
        current = current->next;
    }
}

void print_string_kvp_table(Kvp** table) {
    for (int i = 0; i < SLOTS; i++) {
        if (table[i] == NULL || table[i]->key == NULL) continue;
        printf("\t\t%2d: key: %s\tvalue: %s\tangle: %u\n", i, table[i]->key, (char *) table[i]->value,
               hash_angle(table[i]->key));
    }
}

static double loadfactor(Node* node) {
    return (double) node->load / (double) SLOTS;
}

Kvp *string_kvp(char *key, char *value) {
    Kvp* kvp = init_kvp();
    kvp->key = key;
    kvp->value = value;
    return kvp;
}
