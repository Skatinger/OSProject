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

#if USE_TLS == TRUE
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#endif



// the address of this server (based on its real address)
static struct sockaddr_in server_address;

struct sockaddr_in s_init_address(int port) {
  struct sockaddr_in server;

  memset(&server, '0', sizeof(server)); // zero initialise

  // INADDR_ANY is the actual IP address of this machine
  // htonl converts it into the needed format
  server.sin_addr.s_addr = htonl(INADDR_ANY); // takes address of this computer
  // AF_INET = usual IP protocol family
  server.sin_family = AF_INET;
  server.sin_port = htons(port); // htons converts portNr into needed format

  return server;
}

int s_create_socket(int port) {
    int socket_descriptor = 0;

    // create a new socket and store its id into the var socket_descriptor
    // params mean usual protocols are needed (IP, TCP) and no UDP.
    socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket_descriptor < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    // init the address of this server
    server_address = s_init_address(port);

    // try to bind the socket to this address
    if (bind(socket_descriptor, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
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
    perror("Unable to connect.");
    exit(EXIT_FAILURE);
  } else {
    return descriptor;
  }
}

int s_read(connection_t* con_info) {
  // initialising the buffer with zeros
  bzero(con_info->buffer, BUFFER_SIZE);

  //printf("hello from s_read\n");

  // reading the transmitted data into the buffer
  con_info->data_length = read(con_info->socket_descriptor, con_info->buffer, BUFFER_SIZE -1);
  if (con_info->data_length < 0) {
    perror("Unable to read data from client.");
    return con_info->data_length;
  } else {
    return 0;
  }
}

int s_write(connection_t* con_info, char message[BUFFER_SIZE]) {
  int n = write(con_info->socket_descriptor, message, strlen(message));
  if (n < 0) {
    perror("Unable to write data to client");
    return n;
  } else {
    return 0;
  }
}

// ===== SSL PART FOR FUTURE =========
// partly inspired by a web tutorial :)

#if USE_TLS == TRUE

void s_init_TLS() {
  // load stuff
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

int s_connect_TLS(int connection_descriptor, SSL** tls_to_store) {
  SSL_CTX* ctx = s_create_TLS_context();

  SSL* tls;
  // create a new SSL connection info based on the context
  *tls_to_store = SSL_new(ctx);
  // and 'bind' it to the existing TCP connection socket (an already accepted
  // conncetion of a client!)
  int err;

  if ((err = SSL_set_fd(*tls_to_store, connection_descriptor)) <= 0) {
    s_TLS_error("Joining socket with TLS failed.", FALSE);
  }

  if ((err = SSL_accept(*tls_to_store)) <= 0) {
    printf("%s\n", ERR_error_string(SSL_get_error(*tls_to_store, err), NULL));
    s_TLS_error("Unable to connect (TLS)", FALSE);
    return -1;
  } else {
    return 0;
  }
}

int s_read_TLS(connection_t* con_info) {
  // initialising the buffer with zeros
  bzero(con_info->buffer, BUFFER_SIZE);

  // reading the transmitted data into the buffer
  con_info->data_length = SSL_read(con_info->TLS_descriptor, con_info->buffer, BUFFER_SIZE -1);
  if (con_info->data_length <= 0) {
    s_TLS_error("Unable to read data from TLS client.", FALSE);
    return con_info->data_length - 1;
  } else {
    return 0;
  }
}


int s_write_TLS(connection_t* con_info, char message[BUFFER_SIZE]) {
  int n = SSL_write(con_info->TLS_descriptor, message, strlen(message));
  if (n <= 0) {
    s_TLS_error("Unable to write data to client", FALSE);
    return n == 0 ? -1 : n;
  } else {
    return 0;
  }
}

SSL_CTX* s_create_TLS_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    // negotiates the newest possible version of TLS
    method = SSLv23_server_method();
    if (!method) {
      s_TLS_error("Method assignment failed", TRUE);

    }

    // try to create a new context which will hold certain ECKDATEN for the
    // conncetion
    ctx = SSL_CTX_new(method);
    if (!ctx) {
      s_TLS_error("Unable to create TLS context", TRUE);
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
      s_TLS_error("Certificate file loading failed", FALSE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0 ) {
      s_TLS_error("Private Key file Loading failed.", TRUE);
    }
    return ctx;
}

void s_TLS_error(char* error_msg, int exit_program) {
  int err;
  char buf[ERR_BUF_SIZE];

  // get all SSL error messages
  while ((err = ERR_get_error()) != 0) {
    ERR_error_string_n(err, buf, sizeof(buf));
    printf("*** %s\n", buf);
  }

  perror(error_msg);
  if (exit_program) exit(EXIT_FAILURE);
}

void s_end_TLS(connection_t* con_info) {
  close(con_info->socket_descriptor);
  EVP_cleanup();
  free(con_info);
}


#endif
