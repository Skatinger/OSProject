#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keyvalue.h"

/* ==== structs =====*/
typedef struct keyValuePair {
    char *key;
    char *value;
    struct keyValuePair *next;
} keyValuePair;

typedef struct KVS {
    int size;
    int load;
    struct keyValuePair *table;
} KVS;
/* ==================== */


/* ====== hashing ====== */
int hashFunc (KVS *t, char *key) {
    int h;
    for (h = 0; *key != '\0'; key++) {
        h = (ALPHABET_SIZE * h + *key) % t->size;
    }
    return h;
}

int reHash (KVS *t, int index) {
    int ret;
    int val = index;
    while (val > 0) {
        ret *= val % 10;
    }
    return ret % t->size;
}
/* ====================== */


/* == KVS-access-methods == */
//TODO: FIX (gets caught in loop)
int set(KVS *t, char* key, char* value) {
    if(t->load == t->size) return ERROR;//table is full
    int index = hashFunc(t, value);
    while(t->table[index].key != NULL){
        printf("fuck");
        index += reHash(t, index);
    }
    t->table[index].key = key;
    t->table[index].value = value;
    return SUCCESS;
}

char* get(KVS *t, char *key) {
    int index = hashFunc(t, key);
    while(t->table[index].key != NULL){
        index += reHash(t, index);
    }
    return t->table[index].value;
}

char* del(KVS *t, char *key) {
    int index = hashFunc(t, key);
    while(!strcmp(t->table[index].key, key)){
        index += reHash(t, index);
    }
    char* res = t->table[index].value;
    t->table[index].value = 0;
    t->table[index].key = NULL;
    return res;
}

int replace(KVS* s, char* key, char* value){
    //TODO
    
    return SUCCESS;
    
}
/* ====================== */


/* ==== KVS-Management === */
KVS* create(int size) {
    KVS *newStore = malloc (sizeof (KVS));
    newStore->size = size;
    newStore->table = malloc (size * sizeof (keyValuePair));
    return newStore;
}
//TODO: this is horseradish
void destroy(KVS* store){
    //free KeyValuePairs
    int i;
    keyValuePair *kvp1, *kvp2;
    for(i=0; i < store->size; i++){
        kvp1 = (&store->table[i])->next;
        while (kvp1 != NULL) { kvp2 = kvp1; kvp1 = kvp1->next; free(kvp2->key); free(kvp2); }
        //free((&t->table[i])->key);
    }
    free(store);
}

float loadfactor (KVS *store){
    
    return 1.0;
}
/* ====================== */
