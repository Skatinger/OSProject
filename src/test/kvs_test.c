#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../server/keyvalue.h"

void test(){
    printf("testing KVS:\n");
    KVS *t = create(100);
    printf("KVS created\n");
    printf("initial load factor: %f\n", loadfactor(t));
    set(t, "one", (void*)"two");
    printf("first pair set\n");
    printf("get first pair: one : %s\n", (char*) get(t, "one"));
    set(t, "three", (void*)"four");
    set(t, "five", (void*)"six");
    set(t, "seven", (void*)"eight");
    set(t, "seven", (void*)"eight");
    printKVS(t);
    printf("del one: %s\n", (char*) del(t, "one"));
    printf("deleted object\n");
    printf("del five: %s\n", (char*)del(t, "five"));
    printf("del seven: %s\n", (char*)del(t, "seven"));
    printf("get three: %s\n", (char*)get(t, "three"));
    replace(t, "three", (void*)"nine");
    printf("replace three with nine: three -> %s\n", (char*)get(t, "three"));
    printf("loadfactor: %f\n", loadfactor(t));
    printf("What happens when a nonexistant key is searched?\n");
    printf("Get key whatever: %s\n", (char*)get(t, "whatever"));
    printf("Or when you try to delete it even?\n");
    printf("Del key whatever: %s\n", (char*)del(t, "whatever"));

    destroy(t);
}

int main (){
    test();
    return 0;
}
