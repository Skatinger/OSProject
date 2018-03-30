#include <pthread.h>
#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "server.h"
#include "connectionHandler.h"
#include "../project.h"

int socket;
pthread_t connections[MAX_CONNECTIONS];


int useSSL = FALSE;

int main(int argc, char const *argv[]) {
  socket = createServerSocket(PORT);
  s_listen(socket);

  if (useSSL) {
    init_openssl();
    SSL_CTX* context = create_context();
  }


  return 0;
}
