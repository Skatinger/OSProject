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

int main(int argc, char const *argv[]) {
  // new descriptor ('id') for a socket
  int socket_d =  0;
  // array with all the connection threads
  pthread_t connection_threads[MAX_CONNECTIONS];
  int t_counter = 0; // counting

  socket_d = createServerSocket(PORT);
  s_listen(socket_d);
  int connection_descriptor = 0;


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
      //pthread_join(connection_threads[t_counter-1], ret);
      //printf("continues after thread join\n");
    #endif
  }

  return 0;
}

char* getFirstParam(char* msg) {
  printf("greetings from get1stpar\n");
  static char param[BUFFERSIZE];
  int i, n;
  i = 0; n = strlen(msg);
  while (msg[i] != ' ') {printf("%d:%c\n", i, msg[i]); i++;}
  i++;
  while (msg[i] != ':') {printf("%d:%c\n",  i, msg[i]); strcat(param, &msg[i]); i++;}
  return param;
}

char* getSecondParam(char* msg) {
  static char param[BUFFERSIZE];
  int i, n;
  i = 0; n = strlen(msg);
  while (msg[i] != ':') {i++;}
  i++;
  while (msg[i] != ';' && msg[i] != ':') {strcat(param, &msg[i]); i++;}
  return param;
}

void* handleConnection(void* arg) {
  // casting back to struct holding the data
  connectionInfo* cinf = (connectionInfo*) arg;
  printf("hello from 'child' thread\n");
  int r = s_read(cinf);
  if (r == 0) printf("Reading from socket %d:\n%s\n", cinf->socket_descriptor, cinf->buffer);

  char* username = getFirstParam(cinf->buffer);
  //char* password = getSecondParam(cinf->buffer);
  s_write(cinf, SUCCESS_LOGIN(username));
  return NULL;
}

void* handleTLSConnection(void* arg) {
  return NULL;
}
