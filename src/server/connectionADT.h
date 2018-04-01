#ifndef ADT_H
#define ADT_H

#include <pthread.h>

/**
 * This specifies the primitives needed for the data structure holding
 * the connections.
 */

typedef struct {
  pthread_t thread;
  node_t* next;
  node_t* previous;
} node_t;

typedef struct {
  node_t* head;
  node_t* tail;
} connectionADT;

int add(connectionADT* descriptor, pthread_t thread);

int remove(connectionADT* descriptor, pthread_t thread);

#endif
