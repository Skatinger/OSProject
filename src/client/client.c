#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "client.h"



#if USE_TLS == TRUE
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#endif

/*
 * Since all of these are only used here, it seems to make sense to declare
 * them globally (within file scope, thus <code>static</code>). Since this is
 * a client, there is only one socket to treat anyway.
 */
static struct sockaddr_in server_address;
static int socket_descriptor;
static SSL* tls;

static struct sockaddr_in c_parse_address(int port, char* ip_address) {
  struct sockaddr_in addr;

  memset(&addr, '0', sizeof(addr)); // zero-init

  // convert IP string to usable data
  if(inet_pton(AF_INET, ip_address, &addr.sin_addr)<=0) {
      perror("\n IP conversion error occured\n");
      exit(EXIT_FAILURE);
  }

  // AF_INET = usual IP protocol family
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port); // htons converts portNr into needed format

  return addr;
}

static int c_create_socket(int port) {
  int socket_descriptor = 0;

  // create a new socket
  socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (socket_descriptor < 0) {
      perror("Unable to create socket");
      exit(EXIT_FAILURE);
  }

  return socket_descriptor;
}

static void c_init_TLS() {
  SSL_CTX* ctx;
  int err;
  SSL_library_init();
  SSL_load_error_strings();

  socket_descriptor = c_create_socket(PORT);
  printf("-- Created socket --\n");

  ctx = c_create_context();
  tls = SSL_new(ctx);
}

int c_connect_TLS(char* ip_address) {
  c_init_TLS();
  server_address = c_parse_address(PORT, ip_address);

  if(connect(socket_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
     perror("Socket connecting failed");
     exit(EXIT_FAILURE);
     return 1;
  }

  if (SSL_set_fd(tls, socket_descriptor) <= 0) {
    c_TLS_error("Joining TLS with Socket failed", TRUE);
    return 1;
  }
  printf("-- Joined socket with tls -- \n");

  if (SSL_connect(tls) <= 0) {
    c_TLS_error("TLS connection failed", TRUE);
    return 1;
  }
  printf("-- Connected to server -- \n");
  return 0;
}

int c_send_TLS(char* message) {
  int n = SSL_write(tls, message, strlen(message));
  if (n <= 0) {
    c_TLS_error("Unable to send message to server", FALSE);
    return n - 1;
  } else {
    return 0;
  }
}

int c_receive_TLS(char buffer[BUFFER_SIZE]) {
  // initialising the buffer with zeros
  bzero(buffer, BUFFER_SIZE);
  int n;

  // reading the transmitted data into the buffer
  n = SSL_read(tls, buffer, BUFFER_SIZE -1);
  if (n <= 0) {
    c_TLS_error("Unable to read data from TLS server.", FALSE);
    return n - 1;
  } else {
    buffer[n] = '\0';
    return 0;
  }
}

// for testing only
void c_end_TLS() {
  close(socket_descriptor);
  EVP_cleanup();
  free(tls);
}


static SSL_CTX* c_create_context() {
  SSL_CTX* ctx = NULL;
  int err; char buf[ERR_BUF_SIZE];

  // this will negotiate the newest TLS version with the server
  const SSL_METHOD* method = SSLv23_method();
  if(method == NULL) {
    c_TLS_error("TLS method somehow null...", TRUE);
  }

  // create the context based on the chosen method
  ctx = SSL_CTX_new(method);
  if(!ctx) {
    c_TLS_error("TLS context creation failed.", TRUE);
  }

  /*
  For the moment, certificate verification is off as this can be rather annoying
  to debug and stuff.
  Also, it isn't rally necessary.
   */
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

  //SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

  //SSL_CTX_set_verify_depth(ctx, 4);
  // res = SSL_CTX_load_verify_locations(ctx, "crypto/Fake_CA/ca.crt", NULL);
  // if(!(1 == res)) c_TLS_error("verify locations");

  // set sesible options for this connection
  const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
  SSL_CTX_set_options(ctx, flags);

  return ctx;
}

static void c_TLS_error(char* error_msg, int exit_program) {
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
