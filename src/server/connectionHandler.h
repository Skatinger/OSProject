#ifndef CONNECTIONHANDLER
#define CONNECTIONHANDLER

#include "../project.h"
#define MAX_CONNECTIONS 100


#if USE_TLS == TRUE
  #include <openssl/ssl.h>
#endif

typedef struct connection_t {
  int data_length;                // length of read data
  char buffer[BUFFER_SIZE];   // buffer to read data from this connection
  int socket_descriptor; // the socket for this connection (represented by an int)
  #if USE_TLS == TRUE
    SSL* TLS_descriptor;   // the tls connection
  #endif
} connection_t;

/*
Basics: Every connection has its own thread.
 */

/**
 * Used in a thread that waits for new connections. Once a new request arrives
 * it starts a new handling thread.
 * @param arg void pointer to the socket_d.
 */
void* accept_new_connections(void* arg);
/**
 * Used in a pthread to handle one specific TCP connection.
 * @param arg a connection_t struct (cast to void*)
 */
void* handle_connection(void* arg);

/**
 * Used in a pthread to handle a TLS connection.
 * @param arg a connection_t struct (cast to void*)
 */
void* handle_TLS_connection(void* arg);

#endif
