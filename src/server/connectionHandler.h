#ifndef CONNECTIONHANDLER
#define CONNECTIONHANDLER

#define MAX_CONNECTIONS 100
#define PORT 4567

#define BUFFERSIZE 1024

typedef struct connectionInfo {
  char buffer[1024];
  int socket_descriptor;
  SSL* tls_descriptor;
} connectionInfo;

/*
Basics: Every connection has its own thread.
 */

/**
 * Used in a pthread to handle one specific TCP connection.
 * @param arg [description]
 */
void* handleConnection(void* arg);

void* handleTLSConnection(void* arg);

#endif
