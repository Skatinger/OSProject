#include "../server/key_value_v3.h"
#include "../utils/string_stuff.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

char *random_string() {
    char *string = malloc(11);

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
    for (int i = 0; i < 5000; i++) {
        //char* key = random_string();
        char* key = ss_concat(2, "key", ss_int_to_3digit_string(i));
        unsigned int ang = kvs_hash_angle(key);
        //printf("Inserting %s with angle %u\n", key, ang);
        kvs_set(kvs, key, random_string());
        //print_kvs(kvs);
    }
}

void removesomeofthem(KVS* kvs) {
    int r = 0;
    for (int i = 0; i < 5000; i++) {
        r = rand() % 10;
        if (r) {
            char* key = ss_concat(2, "key", ss_int_to_3digit_string(i));
            kvs_del(kvs, key);
        }
    }
}

void test() {
    printf("testing KVS:\n");
    KVS *t = kvs_create();
    addtwohundredkeys(t);
    printf("KVS created\n");
    //printf("initial load factor: %f\n", loadfactor(t));
    kvs_set(t, "one", (void *) "two");
    printf("first pair set\n");
    printf("get first pair: one : %s\n", (char *) kvs_get(t, "one"));
    printf("Adding 3;4, 5:6, 7:8, 7:8\n");
    kvs_set(t, "three", (void *) "four");
    kvs_set(t, "five", (void *) "six");
    kvs_set(t, "seven", (void *) "eight");
    kvs_set(t, "seven", (void *) "notEight");
    print_kvs(t);
    printf("del one: %s\n", (char *) kvs_del(t, "one"));
    printf("deleted object\n");
    printf("Adding one again\n");
    kvs_set(t, "one", (void *) "newTwo");
    printf("del five: %s\n", (char *) kvs_del(t, "five"));
    printf("del seven: %s\n", (char *) kvs_del(t, "seven"));
    printf("get three: %s\n", (char *) kvs_get(t, "three"));
    kvs_replace(t, "three", (void *) "nine");
    printf("replace three with nine: three -> %s\n", (char *) kvs_get(t, "three"));
    //printf("loadfactor: %f\n", loadfactor(t));
    printf("What happens when a nonexistant key is searched?\n");
    printf("Get key whatever: %s\n", (char *) kvs_get(t, "whatever"));
    printf("Or when you try to delete it even?\n");
    printf("Del key whatever: %s\n", (char *) kvs_del(t, "whatever"));
    //printf("adding like 200 keys\n");
    printf("Adding stuff to test get by val\n");
    kvs_set(t, "two", "newTwo");
    kvs_set(t, "anotherTwo", "newTwo");
    kvs_replace(t, "three", "newTwo");
    printf("Getting keys for val newTwo, should be two, anotherTwo, three, one \n");
    printf("%s\n", keys_for_string_values(t, "newTwo"));

    kvs_destroy(t);
}

int node_test() {
    LL *list = create_LL();
    Node *middle = create_node(MAX_ADDRESS / 2);
    insert_node(list, middle);
    Node *seventeen = create_node(17);
    Node *three_fourths = create_node((int) (0.75 * MAX_ADDRESS));
    Node *one_quarter = create_node(MAX_ADDRESS / 4);
    Node *threehundredsomething = create_node(367);
    insert_node(list, seventeen);
    insert_node(list, three_fourths);
    insert_node(list, one_quarter);
    insert_node(list, threehundredsomething);

    kvs_print_tables_list(list, TRUE);

    printf("Now removing node at 367\n");
    remove_node(list, 367);

    kvs_print_tables_list(list, TRUE);

    printf("Reinsertign 367 and deleting end\n");
    insert_node(list, create_node(367));
    remove_node(list, MAX_ADDRESS);


    kvs_print_tables_list(list, TRUE);
    printf("Now deleting the first element in the list\n");

    remove_node(list, 17);
    kvs_print_tables_list(list, TRUE);

    printf("Adding kvps to 1/4-node\n");
    Kvp *first = string_kvp("KEY", "VALUE");
    Kvp *second = string_kvp("1234567890", "wahtev");
    Kvp *third = string_kvp("hello", "velo");


    insert_into_node(one_quarter, first);
    insert_into_node(one_quarter, second);
    insert_into_node(one_quarter, third);

    kvs_print_tables_list(list, TRUE);

    printf("Now removing the hello again\n");
    remove_from_node(one_quarter, "hello");

    kvs_print_tables_list(list, TRUE);

    printf("adding two more values and replacing 1234567890 value with lol\n");
    Kvp *four = string_kvp("idk", "kjshfjks");
    Kvp *five = string_kvp("cinq", "5");
    insert_into_node(one_quarter, four);
    insert_into_node(one_quarter, five);
    replace_in_node(one_quarter, "1234567890", "lol");

    kvs_print_tables_list(list, TRUE);
    printf("lfactor of 1/4: %f\n", loadfactor(one_quarter));
    printf("lfactor of 1/4, get by list %f\n", loadfactor(find_node_after(list, MAX_ADDRESS / 4)));
}

int main(int argc, char const *argv[]) {
    //init_logger("kvs3test", FALSE);
    KVS* test = kvs_create();
    // node_test();
    clock_t before = clock();
    //test();

    addtwohundredkeys(test);
    kvs_print_tables_list(test->key_nodes, TRUE);
    printf("==================================");
    removesomeofthem(test);
    kvs_print_tables_list(test->key_nodes, TRUE);
    clock_t after = clock();
    double time = ((double) (after - before)) / CLOCKS_PER_SEC;
    printf("Difference: %f\n", time);

    return 0;
}
