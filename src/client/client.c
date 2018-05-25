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
#include "client_requests.h"
#include "../utils/logger.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

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

static int c_create_socket() {
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

  // lib-related init stuff
  SSL_library_init();
  SSL_load_error_strings();

  socket_descriptor = c_create_socket();
  logger("-- Created socket --", INFO);

  ctx = c_create_context();
  logger("-- Created context --", INFO);

  tls = SSL_new(ctx);
  logger("-- Created TLS --", INFO);
}

int c_connect_TLS(char* ip_address) {
  c_init_TLS();
  server_address = c_parse_address(PORT, ip_address);

  // try to connect using TCP
  if(connect(socket_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
     perror("Socket connecting failed");
     exit(EXIT_FAILURE);
     return 1;
  }

  // and join it to the TLS interface
  if (SSL_set_fd(tls, socket_descriptor) <= 0) {
    c_TLS_error("Joining TLS with Socket failed", TRUE);
    return 1;
  }
  logger("-- Joined socket with tls --", INFO);

  if (SSL_connect(tls) <= 0) {
    c_TLS_error("TLS connection failed", TRUE);
    return 1;
  }
  logger("-- Connected to server --", INFO);
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
    buffer[n] = 0;
    logger(buffer, INFO);
    return 0;
  }
}

void c_end_TLS() {
  c_send_TLS(BYE);
  close(socket_descriptor);
  EVP_cleanup();
  free(tls);
}

static SSL_CTX* c_create_context() {
  // set up the necessary context, i.e. settings for the TLS session
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
  Certificate verification is off, getting certificates is annoying and
  not really necessary for this project.
   */
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

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
    logger(ss_concat(2, "***", buf), LOGERROR);
  }

  perror(error_msg);
  if (exit_program) exit(EXIT_FAILURE);
}
