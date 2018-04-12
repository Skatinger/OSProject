#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/socket.h>
#include <string.h>
#include "server.h"
#include "connectionHandler.h"
#include "serverResponses.h"

#if USE_TLS == TRUE
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


  socket_d = s_create_socket(PORT);
  s_listen(socket_d);
  pthread_t pid;

  printf("Listening for new connections now\n");
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
    connection_t* con_info = malloc(sizeof(connection_t));
    con_info->socket_descriptor = connection_descriptor;
    // initialising the buffer with zeros
    //bzero(con_info->buffer, BUFFER_SIZE);
    char buf[BUFFER_SIZE];
    bzero(buf, BUFFER_SIZE);
    strcpy(con_info->buffer, buf);
    con_info->data_length = 0;

    printf("created con_info\n");
    printf("socket: %d\n", con_info->socket_descriptor);
    //printf("buffer: %s\n", con_info->buffer);
    printf("buffer: %s\n", con_info->buffer);
    #if USE_TLS
      SSL* tls = NULL;
      s_init_TLS();
      s_connect_TLS(connection_descriptor, &tls);
      con_info->TLS_descriptor = tls;
      printf("thread creation: %d\n", pthread_create(&connection_threads[t_counter++], NULL, handle_TLS_connection, (void*)con_info));
    #else
      printf("thread creation: %d\n", pthread_create(&connection_threads[t_counter++], NULL, handle_connection, (void*)con_info));
    #endif
  }
}

// will be used in handler
char* getFirstParam(char* msg);
char* getSecondParam(char* msg);


void* handle_connection(void* arg) {
  // casting back to struct holding the data
  connection_t* con_info = (connection_t*) arg;
  //printf("hello from 'child' thread\n");
  int r = s_read(con_info);
  if (r == 0) printf("Reading from socket %d:\n%s\n", con_info->socket_descriptor, con_info->buffer);

  char* username = getFirstParam(con_info->buffer);
  //char* password = getSecondParam(con_info->buffer);
  s_write(con_info, SUCCESS_LOGIN(username));
  // if that test was successful, kill the program
  //pthread_cancel(mainThread);
  return NULL;
}

#if USE_TLS == TRUE
  void* handle_TLS_connection(void* arg) {

    connection_t* con_info = (connection_t*) arg;

    int r = s_read_TLS(con_info);
    if (r == 0) {
      printf("Reading from socket %d:\n%s\n", con_info->socket_descriptor, con_info->buffer);

      char* username = getFirstParam(con_info->buffer);
      //char* password = getSecondParam(con_info->buffer);
      s_write_TLS(con_info, SUCCESS_LOGIN(username));
      s_end_TLS(con_info);
    }
    // if that test was successful, kill the program
    //pthread_cancel(mainThread);
    return NULL;

  }
#endif

//============ PARSING FUNCTIONS ===================
// should be moved into a separate file once complete, these
// are more for testing purposes.
//
char* getFirstParam(char* msg) {
  static char param[BUFFER_SIZE];
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
  static char param[BUFFER_SIZE];
  int i, n, k;
  i = 0; n = strlen(msg);

  // skip the command and first arg
  while (msg[i] != ':') {i++;}
  i++; k = i; // k = beginning of the second arg
  // loop until end or third arg begins
  while (msg[i] != ';' && msg[i] != ':') {param[i-k] = msg[i]; i++;}
  return param;
}

char* getThirdParam(char* msg) {
  static char param[BUFFER_SIZE];
  int i, n, k;
  i = 0; n = strlen(msg);

  while(msg[i] != ':') {i++;}
  i++; // loop over the secon arg as well
  while(msg[i] != ':') {i++;}
  i++; k = i;
  while (msg[i] != ';') {param[i-k] = msg[i]; i++;}
  return param;
}
