#include <pthread.h>
#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "server.h"
#include "connectionHandler.h"
#include "../project.h"

int socket;
pthread_t connection_threads[MAX_CONNECTIONS];


int useSSL = FALSE;

int main(int argc, char const *argv[]) {
  socket = createServerSocket(PORT);
  s_listen(socket);
  s_connect(socket);


  if (useSSL) {
    init_openssl();
    SSL_CTX* context = create_context();
    configure_context(context);
    SSL* tls;

  }


  return 0;
}


void* handleConnection(void* arg) {

}

void* handleTLSConnection(void* arg) {

}
