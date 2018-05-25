#ifndef CONNECTIONHANDLER
#define CONNECTIONHANDLER

#include <openssl/ssl.h>
#include "../project.h"

#define MAX_IDLE 7200 // 2h
#define RETRIAL_TIME 10


/*
Basics: Every connection has its own thread.
 */

/**
 * Used in a thread that waits for new connections. Once a new request arrives
 * it starts a new handling thread.
 * @param arg void pointer to the socket_d.
 */
static void* accept_new_connections(void* arg);

/**
 * Used in a pthread to handle a TLS connection.
 * @param arg a connection_t struct (cast to void*)
 */
static void* handle_TLS_connection(void* arg);

#endif
