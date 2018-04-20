#ifndef CONNECTIONHANDLER
#define CONNECTIONHANDLER

#include "../project.h"
#define MAX_CONNECTIONS 100
#define MAX_IDLE 7200 // 2h
#define RETRIAL_TIME 10


#if USE_TLS == TRUE
  #include <openssl/ssl.h>
#endif


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
