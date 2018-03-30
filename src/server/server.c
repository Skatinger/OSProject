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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "server.h"
#include "../project.h"

struct sockaddr_in serverAddress;

struct sockaddr_in makeServerAddress(int port) {
  struct sockaddr_in server;

  server.sin_addr.s_addr = htonl(INADDR_ANY); // takes address of this computer
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  return server;
}

int createServerSocket(int port) {
    int socket_descriptor;

    socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket_descriptor < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    serverAddress = makeServerAddress(port);

    if (bind(socket_descriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Unable to bind socket");
        exit(EXIT_FAILURE);
    }
    return socket_descriptor;
}

int s_listen(int socket_descriptor) {
  if (listen(socket_descriptor, MAX_CONNECTION_QUEUE) < 0) {
    perror("Unable to listen.");
    exit(EXIT_FAILURE);
  } else {
    return 0;
  }
}

int s_connect(int socket_descriptor) {
  struct sockaddr_in* clientAddress;
  unsigned int len = sizeof(clientAddress);
  int descriptor = accept(socket_descriptor, (struct sockaddr *) &clientAddress, &len);
  if (descriptor < 0){
    perror("Unable to connect");
    exit(EXIT_FAILURE);
  } else {
    return descriptor;
  }
}

SSL* s_connectTLS(int conncetionDescriptor, SSL_CTX* ctx) {
  SSL* ssl;
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, client);

  if (SSL_accept(ssl) <= 0) {
    perror("Unable to connect (TLS)");
    exit(EXIT_FAILURE);
  } else {
      return ssl;
  }
}


// ==== TLS STUFF
//
void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	     perror("Unable to create SSL context");
	     ERR_print_errors_fp(stderr);
	     exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, CERTIFICATE_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	       exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	      exit(EXIT_FAILURE);
    }
}


// int main(int argc, char *argv[]){
//     int listenfd = 0, connfd = 0;
//     struct sockaddr_in serv_addr;
//
//     char sendBuff[1025];
//     time_t ticks;
//
//     listenfd = socket(AF_INET, SOCK_STREAM, 0);
//     memset(&serv_addr, '0', sizeof(serv_addr));
//     memset(sendBuff, '0', sizeof(sendBuff));
//
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     serv_addr.sin_port = htons(5000);
//
//     bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//
//     listen(listenfd, 10);
//
//     while(1)
//     {
//         connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
//
//         ticks = time(NULL);
//         snprintf(sendBuff, sizeof(sendBuff), "Hello member of Group 6. Current date and time is: %.24s\r\n", ctime(&ticks));
//         write(connfd, sendBuff, strlen(sendBuff));
//
//         close(connfd);
//         //leave this, otherwise CPU load gets higher than necessary
//         sleep(1);
//      }
// }
