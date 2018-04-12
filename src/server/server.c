#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "server.h"
#include "../project.h"

#if USE_SSL == TRUE
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#endif



// the address of this server (based on its real address)
struct sockaddr_in serverAddress;

struct sockaddr_in makeServerAddress(int port) {
  struct sockaddr_in* server;

  server = malloc(sizeof(struct sockaddr_in));

  // INADDR_ANY is the actual IP address of this machine
  // htonl converts it into the needed format
  server->sin_addr.s_addr = htonl(INADDR_ANY); // takes address of this computer
  // AF_INET = usual IP protocol family
  server->sin_family = AF_INET;
  server->sin_port = htons(port); // htons converts portNr into needed format

  return *server;
}

int createServerSocket(int port) {
    int socket_descriptor = 0;

    // create a new socket and store its id into the var socket_descriptor
    // params mean usual protocols are needed (IP, TCP) and no UDP.
    socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket_descriptor < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    // init the address of this server
    serverAddress = makeServerAddress(port);

    // try to bind the socket to this address
    if (bind(socket_descriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Unable to bind socket");
        exit(EXIT_FAILURE);
    }
    return socket_descriptor;
}

int s_listen(int socket_descriptor) {
  // MAX_CONNECTION_QUEUE is the max. number of connections that can be queued
  // if the server is busy, apparently this is usually 5
  // (defined in header file)
  if (listen(socket_descriptor, MAX_CONNECTION_QUEUE) < 0) {
    perror("Unable to listen.");
    exit(EXIT_FAILURE);
  } else {
    return 0;
  }
}

int s_connect(int socket_descriptor) {
  // place where the address of the connecting client will be saved
  struct sockaddr_in* clientAddress;
  unsigned int len = sizeof(clientAddress); // and its length

  // blocks as long as there is no connection attempt by a client
  // once there is one, connection is established via a 'new socket', whose
  // 'id' is stored in descriptor
  int descriptor = accept(socket_descriptor, (struct sockaddr *) &clientAddress, &len);
  if (descriptor < 0){
    perror("Unable to connect");
    exit(EXIT_FAILURE);
  } else {
    return descriptor;
  }
}

int s_read(connectionInfo* cinf) {
  // initialising the buffer with zeros
  bzero(cinf->buffer, BUFFERSIZE);

  //printf("hello from s_read\n");

  // reading the transmitted data into the buffer
  cinf->data_length = read(cinf->socket_descriptor, cinf->buffer, BUFFERSIZE -1);
  if (cinf->data_length < 0) {
    perror("Unable to read data from client.");
    return cinf->data_length;
  } else {
    return 0;
  }
}

int s_write(connectionInfo* cinf, char message[BUFFERSIZE]) {
  int n = write(cinf->socket_descriptor, message, strlen(message));
  if (n < 0) {
    perror("Unable to write data to client");
    return n;
  } else {
    return 0;
  }
}

// ===== SSL PART FOR FUTURE =========
// partly inspired by a web tutorial :)

#if USE_SSL == TRUE
// SSL* s_connectTLS(int connection_descriptor, SSL_CTX* ctx) {
//   SSL* ssl;
//   // create a new SSL connection info based on the context
//   ssl = SSL_new(ctx);
//   // and 'bind' it to the existing TCP connection socket (an already accepted
//   // conncetion of a client!)
//   SSL_set_fd(ssl, connection_descriptor);
//
//   if (SSL_accept(ssl) <= 0) {
//     perror("Unable to connect (TLS)");
//     exit(EXIT_FAILURE);
//   } else {
//       return ssl;
//   }
// }
//
int s_connectTLS(int connection_descriptor, SSL_CTX* ctx, SSL** ssl_to_store) {
  SSL* ssl;
  // create a new SSL connection info based on the context
  *ssl_to_store = SSL_new(ctx);
  // and 'bind' it to the existing TCP connection socket (an already accepted
  // conncetion of a client!)
  SSL_set_fd(*ssl_to_store, connection_descriptor);
  int err;
  if ((err = SSL_accept(*ssl_to_store)) <= 0) {
    printf("%s\n", ERR_error_string(SSL_get_error(*ssl_to_store, err), NULL));
    char buf[256];
    while ((err = ERR_get_error()) != 0) {
      ERR_error_string_n(err, buf, sizeof(buf));
      printf("*** %s\n", buf);
   }
    perror("Unable to connect (TLS)");

    exit(EXIT_FAILURE);
  } else {
      return 1;
  }
}

int s_readTLS(connectionInfo* cinf) {
  // initialising the buffer with zeros
  bzero(cinf->buffer, BUFFERSIZE);

  // reading the transmitted data into the buffer
  cinf->data_length = SSL_read(cinf->tls_descriptor, cinf->buffer, BUFFERSIZE -1);
  if (cinf->data_length < 0) {
    perror("Unable to read data from TLS client.");
    return cinf->data_length;
  } else {
    return 0;
  }
}


int s_writeTLS(connectionInfo* cinf, char message[BUFFERSIZE]) {
  int n = SSL_write(cinf->tls_descriptor, message, strlen(message));
  if (n <= 0) {
    perror("Unable to write data to client");
    return n == 0 ? -1 : n;
  } else {
    return 0;
  }
}


void init_openssl() {
  // load stuff
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    // negotiates the newest possible version of TLS
    method = SSLv23_server_method();

    // try to create a new context which will hold certain ECKDATEN for the
    // conncetion
    ctx = SSL_CTX_new(method);
    if (!ctx) {
	     perror("Unable to create SSL context");
	     ERR_print_errors_fp(stderr);
	     exit(EXIT_FAILURE);
    }

    // no idea yet what this is, will have to check man
    SSL_CTX_set_ecdh_auto(ctx, 1);

    // for the moment, verification is off to make debugging less annoying.
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

    //SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL);

    // Set the key and certificate
    // I used a self-signed certificate as I didn't wanna pay for one
    // the file locations are defined in the header.
    if (SSL_CTX_use_certificate_file(ctx, CERTIFICATE_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	       exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	      exit(EXIT_FAILURE);
    }

    return ctx;
}
#endif
