#include <semaphore.h>

#include "../server/access_handler.h"
#include "../server/key_value_v3.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



static KVS* kvs;

void *reader1_fct(void* test) {
    printf("first reader reads: %s \n", ah_reader((char*)test, GET));
	pthread_exit(0);
}

void *reader2_fct(void* test) {
    printf("second reader reads: %s \n", ah_reader((char*)test, GET));
	pthread_exit(0);
}

int main(int argc, char const *argv[]) {

    pthread_t reader1, reader2;
    void** ret = malloc(sizeof(char*));
    char* test = "test";

    kvs = kvs_create();
    kvs_set(kvs, test, "bla");
    printf("key: %s, value: %s \n", test, (char *)kvs_get(kvs, test));

    pthread_create(&reader1, NULL, reader1_fct, test);
    pthread_create(&reader2, NULL, reader2_fct, test);
    pthread_join(reader1, ret);
    pthread_join(reader2, ret);

    return 0;
}
