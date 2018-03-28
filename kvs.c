#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keyvalue.h"



/* ====== hashing ====== */
int hashFunc (KVS *t, char *key) {
    int h;
    char* tmp = key;
    for (h = 0; *key != '\0'; key++) {
        h = (ALPHABET_SIZE * h + *key) % t->size;
    }
    printf("hashvalue of %s: %d\n", tmp, h);
    return h;
}

int reHash (KVS *t, int index) {

    long int ret = index + 2;
    while (ret < BIGPRIME_1) {
        ret *= ret;
    }
    ret = ret % BIGPRIME_1;
    while (ret < BIGPRIME_2) {
        ret *= 7;
    }
    ret = ret % BIGPRIME_2;
    printf("reHashValue of index %i is %li\n", index, ret);
    return ret;
    //return 1;
}
/* ====================== */

/* == KVS-access-methods == */
int set(KVS *t, char* key, char* value) {
    if(t->load == t->size) return STORAGE_FULL_ERROR;//table is full
    int index = hashFunc(t, key);
    //TODO: FIX (gets caught in loop)
    //printf("index: %d has value %s\n", index, t->table[index].key);
    
    while(t->table[index].key != NULL){
        index += reHash(t, index);
        index = index % t->size;
    }
    t->table[index].key = key;
    t->table[index].value = value;
    printf("Set key: %s, and value: %s\n", t->table[index].key, t->table[index].value);
    t->load++;
    return SUCCESS;
}

char* get(KVS *t, char *key) {
    int index = hashFunc(t, key);
    
    // store first index to detect cycles
    int tmp = index;
    
    while(t->table[index].key != NULL && strcmp(t->table[index].key, key)){
        index += reHash(t, index);
        index = index % t->size;
        if (index == tmp) {
            // back at beginning, stop
            return KEY_NOT_FOUND_ERRORmsg;
        }
    }
    return t->table[index].key!=NULL ? t->table[index].value : KEY_NOT_FOUND_ERRORmsg;
}
//TODO throws segFault
char* del(KVS *t, char *key) {
    if (t->load == 0) return STORAGE_EMPTY_ERROR;
    int index = hashFunc(t, key);
    int tmp = index;
    while(t->table[index].key != NULL && strcmp(t->table[index].key, key)){
        index += reHash(t, index);
        index = index % t->size;
        if (index == tmp) {
            return KEY_NOT_FOUND_ERRORmsg;
        }
    }
    char* res = t->table[index].key!=NULL ? t->table[index].value : KEY_NOT_FOUND_ERRORmsg;
    t->table[index].value = NULL;
    t->table[index].key = NULL;
    t->load--;
    return res;
}

int replace(KVS* t, char* key, char* value){
    //TODO
    int index = hashFunc(t, key);
    int tmp = index;
    
    while(t->table[index].key != NULL && strcmp(t->table[index].key, key)){
        index += reHash(t, index);
        index = index % t->size;
        if (index == tmp) {
            return KEY_NOT_FOUND_ERROR;
        }
    }
    if (t->table[index].key != NULL) {
        t->table[index].value = value;
        return SUCCESS;
    } else {
        return KEY_NOT_FOUND_ERROR;
    }
    
    
}
/* ====================== */

/* ==== KV Pair methods == */
keyValuePair initKVP() {
    keyValuePair *kvp = malloc(sizeof(keyValuePair));
    kvp->key = NULL;
    kvp->value = NULL;
    kvp->next = NULL;
    return *kvp;
}

/* ==== KVS-Management === */
KVS* create(int size) {
    KVS *newStore = malloc (sizeof (KVS));
    newStore->size = size;
    newStore->load = 0;
    newStore->table = malloc (size * sizeof (keyValuePair));
    for (int i = 0; i < size; i++) {
        newStore->table[i] = initKVP();
    }
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
    return (float) store->load/ (float) store->size;
}
/* ====================== */

/* ==== ERROR HANDLING === */





