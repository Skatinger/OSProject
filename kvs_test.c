#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "keyvalue.h"

void test(){
    printf("testing KVS:\n");
    KVS *t = create(100);
    printf("KVS created\n");
    printf("initial load factor: %f\n", loadfactor(t));
    set(t, "one", "two");
    printf("first pair set\n");
    printf("get first pair: one : %s\n", get(t, "one"));
    set(t, "three", "four");
    set(t, "five", "six");
    set(t, "seven", "eight");
    printf("del one: %s\n", del(t, "one"));
    printf("deleted object\n");
    printf("del five: %s\n", del(t, "five"));
    printf("del seven: %s\n", del(t, "seven"));
    printf("get three: %s\n", get(t, "three"));
    replace(t, "three", "nine");
    printf("replace three with nine: three -> %s\n", get(t, "three"));
    printf("loadfactor: %f\n", loadfactor(t));
    printf("What happens when a nonexistant key is searched?\n");
    printf("Get key whatever: %s", get(t, "whatever"));
    destroy(t);
}

int main (){
    test();
    return 0;
}
