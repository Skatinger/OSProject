#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "keyvalue.h"

void test(){
    printf("testing KVS:\n");
    KVS *t = create(100);
    set(t, "one", "two");
    set(t, "three", "four");
    set(t, "five", "six");
    set(t, "seven", "eight");
    printf("del one: %s\n", del(t, "one"));
    printf("del five: %s\n", del(t, "five"));
    printf("del seven: %s\n", del(t, "seven"));
    printf("get three: %s\n", get(t, "three"));
    replace(t, "three", "nine");
    printf("replace three with nine: three -> %s\n", get(t, "nine"));
    printf("loadfactor: %f\n", loadfactor(t));
    destroy(t);
}

int main (int argc, char *argv[]){
    test();
    return 0;
}
