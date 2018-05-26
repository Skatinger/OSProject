#include <semaphore.h>

#include "../server/access_handler.h"
#include "../server/key_value_v3.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



static KVS* kvs;

void *reader_fct(void* test) {
    printf("reader reads: %s \n", ah_reader((char*)test, GET));
    pthread_exit(0);
}

void *writer_fct(void* test) {
    ah_writer((char*)test, "writer", UPD);
    printf("writer overwrites with: %s \n", (char *)kvs_get(kvs, test));
    pthread_exit(0);
}

int main(int argc, char const *argv[]) {

    pthread_t reader, writer;
    void** ret = malloc(sizeof(char*));
    char* test = "test";

    kvs = kvs_create();
    kvs_set(kvs, test, "bla");
    printf("key: %s, value: %s \n", test, (char *)kvs_get(kvs, test));

    pthread_create(&reader, NULL, reader_fct, test);
    pthread_create(&writer, NULL, writer_fct, test);
    pthread_join(reader, ret);
    pthread_join(writer, ret);

    return 0;
}