#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define KEY_T char*
#define VALUE_T char*
#include "../server/key_value.h"

char* random_string() {
    char* string = malloc(11);

    for (int i = 0; i < 10; i++) {
        int r = rand();
        r = r % 90;
        r += 33;
        string[i] = (char) r;
    }
    string[10] = 0;
    return string;
}

void addtwohundredkeys(KVS* kvs) {
    for (int i = 0; i < 200; i++) {
      char* key = random_string();
        //printf("Inserting %s with angle %u\n", key, ang);
        set(kvs, key, random_string());
        //print_kvs(kvs);
    }
}

void test(){
    printf("testing KVS:\n");
    KVS *t = create(512 * 32768);
    addtwohundredkeys(t);
    // printf("KVS created\n");
    // printf("initial load factor: %f\n", loadfactor(t));
    // set(t, "one", (void*)"two");
    // printf("first pair set\n");
    // printf("get first pair: one : %s\n", (char*) get(t, "one"));
    // set(t, "three", (void*)"four");
    // set(t, "five", (void*)"six");
    // set(t, "seven", (void*)"eight");
    // set(t, "seven", (void*)"eight");
    // printKVS(t);
    // printf("del one: %s\n", (char*) del(t, "one"));
    // printf("deleted object\n");
    // printf("Adding one again\n");
    // set(t, "one", (void*) "newTwo");
    // printf("del five: %s\n", (char*)del(t, "five"));
    // printf("del seven: %s\n", (char*)del(t, "seven"));
    // printf("get three: %s\n", (char*)get(t, "three"));
    // replace(t, "three", (void*)"nine");
    // printf("replace three with nine: three -> %s\n", (char*)get(t, "three"));
    // printf("loadfactor: %f\n", loadfactor(t));
    // printf("What happens when a nonexistant key is searched?\n");
    // printf("Get key whatever: %s\n", (char*)get(t, "whatever"));
    // printf("Or when you try to delete it even?\n");
    // printf("Del key whatever: %s\n", (char*)del(t, "whatever"));
    // printKVS(t);
    // destroy(t);
}

int main (){
  clock_t before = clock();
  test();
  clock_t after = clock();
  double time = ((double) (after-before))/ CLOCKS_PER_SEC;
  printf("Difference: %f\n", time);
    return 0;
}
