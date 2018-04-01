#define USE_SSL FALSE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/socket.h>
#include <string.h>
#include "server.h"
#include "connectionHandler.h"
#include "../project.h"
#include "serverResponses.h"

#if USE_SSL
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#endif


// TODO: synchronization (make sure threads dont overwrite counter etc)
// TODO: find a useful data structure for saving theses threads (cf. connectionADT)
// TODO: sensibly handle a connection (read requests, treat them (perhaps via another
// implementation file), send a reply)
// TODO: implement an access control that determines when a connection may or may
// not access the KVS.

int main(int argc, char const *argv[]) {
  // new descriptor ('id') for a socket
  int socket_d =  0;

  char c;

  socket_d = createServerSocket(PORT);
  s_listen(socket_d);
  pthread_t pid;

  // new thread that waits for connections
  pthread_create(&pid, NULL, accept_new_connections, (void *)&socket_d);

  // mechanism to stop the server
  scanf("%c\n", &c);
  if (c == 'c') {
    printf("Stopping.\n");
    exit(0);
  }


  return 0;
}

void* accept_new_connections(void* arg) {
  int socket_d = *(int*)arg;
  int connection_descriptor = 0;
  // array with all the connection threads
  pthread_t connection_threads[MAX_CONNECTIONS];
  int t_counter = 0; // counting

  // entering loop
  while(TRUE) {
    void** ret;
    connection_descriptor = s_connect(socket_d);
    printf("connected to client\n");
    connectionInfo* cinf = malloc(sizeof(connectionInfo));
    cinf->socket_descriptor = connection_descriptor;
    // initialising the buffer with zeros
    //bzero(cinf->buffer, BUFFERSIZE);
    char buf[BUFFERSIZE];
    bzero(buf, BUFFERSIZE);
    strcpy(cinf->buffer, buf);
    cinf->data_length = 0;

    printf("created cinf\n");
    printf("socket: %d\n", cinf->socket_descriptor);
    //printf("buffer: %s\n", cinf->buffer);
    printf("buffer: %s\n", cinf->buffer);
    #if USE_SSL
      init_openssl();
      SSL_CTX* context = create_context();
      configure_context(context);
      SSL* ssl = s_connectTLS(connection_descriptor, context);
      cinf->tls_descriptor = ssl;

      printf("thread creation: %d\n", pthread_create(&connection_threads[t_counter++], NULL, handleTLSConnection, (void*)cinf));
    #else
      printf("thread creation: %d\n", pthread_create(&connection_threads[t_counter++], NULL, handleConnection, (void*)cinf));
    #endif
  }
}

// will be used in handler
char* getFirstParam(char* msg);
char* getSecondParam(char* msg);


void* handleConnection(void* arg) {
  // casting back to struct holding the data
  connectionInfo* cinf = (connectionInfo*) arg;
  printf("hello from 'child' thread\n");
  int r = s_read(cinf);
  if (r == 0) printf("Reading from socket %d:\n%s\n", cinf->socket_descriptor, cinf->buffer);

  char* username = getFirstParam(cinf->buffer);
  //char* password = getSecondParam(cinf->buffer);
  s_write(cinf, SUCCESS_LOGIN(username));
  // if that test was successful, kill the program
  //pthread_cancel(mainThread);
  return NULL;
}

void* handleTLSConnection(void* arg) {
  return NULL;
}


//============ PARSING FUNCTIONS ===================
// should be moved into a separate file once complete, these
// are more for testing purposes.
//
char* getFirstParam(char* msg) {
  static char param[BUFFERSIZE];
  int i, n, k;
  i = 0; n = strlen(msg);

  // skip the 'command'
  while (msg[i] != ' ') {i++;}
  i++; k = i; // k = beginning index of the parameter
  // copy the first argument (i.e. until the second is reached, or the end
  // if there is only one)
  while (msg[i] != ':' && msg[i] != ';') {param[i-k] = msg[i]; i++;}
  // null - terminate --> nicest thing about C
  param[i+1] = '\0';

  return param;
}

char* getSecondParam(char* msg) {
  static char param[BUFFERSIZE];
  int i, n, k;
  i = 0; n = strlen(msg);
  while (msg[i] != ':') {i++;}
  i++; k = i;
  while (msg[i] != ';' && msg[i] != ':') {param[i-k] = msg[i]; i++;}
  return param;
}
