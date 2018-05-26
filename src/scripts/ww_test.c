#include <semaphore.h>

#include "../server/access_handler.h"
#include "../server/key_value_v3.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



static KVS* kvs;

void *writer1_fct(void* test) {
    ah_writer(test, "first", UPD);
    printf("first writer overwrites with: %s \n", (char *)kvs_get(kvs, test));
    pthread_exit(0);
}

void *writer2_fct(void* test) {
    ah_writer(test, "second", UPD);
    printf("second writer overwrites with: %s \n", (char *)kvs_get(kvs, test));
    pthread_exit(0);
}

int main(int argc, char const *argv[]) {

    pthread_t writer1, writer2;
    void** ret = malloc(sizeof(char*));
    char* test = "test";

    kvs = kvs_create();
    kvs_set(kvs, test, "bla");
    printf("key: %s, value: %s \n", test, (char *)kvs_get(kvs, test));

    pthread_create(&writer1, NULL, writer1_fct, test);
    pthread_create(&writer2, NULL, writer2_fct, test);
    pthread_join(writer1, ret);
    pthread_join(writer2, ret);

    return 0;
}