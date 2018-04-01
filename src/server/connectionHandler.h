#ifndef CONNECTIONHANDLER
#define CONNECTIONHANDLER

#define MAX_CONNECTIONS 100
#define PORT 4567

#define BUFFERSIZE 1024

typedef struct connectionInfo {
  int data_length;                // length of read data
  char buffer[BUFFERSIZE];   // buffer to read data from this connection
  int socket_descriptor; // the socket for this connection (represented by an int)
  #if USE_SSL
  SSL* tls_descriptor;   // the ssl connection
  #endif
} connectionInfo;

/*
Basics: Every connection has its own thread.
 */

/**
 * Used in a pthread to handle one specific TCP connection.
 * @param arg a connectionInfo struct (cast to void*)
 */
void* handleConnection(void* arg);

/**
 * Used in a pthread to handle a TLS connection.
 * @param arg a connectionInfo struct (cast to void*)
 */
void* handleTLSConnection(void* arg);

#endif
