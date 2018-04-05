#define USE_SSL TRUE

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "client.h"
#include "../server/serverResponses.h"
#include "../project.h"

#if USE_SSL == TRUE
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#endif

void handleFailure(char* msg) {printf("failure: %s\n", msg);}

void init_openssl_library(void){
  (void)SSL_library_init();

  SSL_load_error_strings();
}

int main(int argc, char *argv[]) {
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    long res; int err;

    if(argc != 2) {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    printf("Created socket\n");

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(50000);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    printf("create server address\n");

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\n Error : Connect Failed \n");
       return 1;
    }

    printf("connected socket\n");

//============ TLS test part =======================
    #if USE_SSL == TRUE
      SSL_CTX* ctx = NULL;
      BIO *web = NULL, *out = NULL;
      SSL *ssl = NULL;

      init_openssl_library();

      const SSL_METHOD* method = SSLv23_method();
      if(!(NULL != method)) handleFailure("method");

      ctx = SSL_CTX_new(method);
      if(!(ctx != NULL)) handleFailure("new ctx");

      /* Cannot fail ??? */
      SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
      //SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);


      /* Cannot fail ??? */
      //SSL_CTX_set_verify_depth(ctx, 4);

      /* Cannot fail ??? */
      const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
      SSL_CTX_set_options(ctx, flags);

      // res = SSL_CTX_load_verify_locations(ctx, "crypto/Fake_CA/ca.crt", NULL);
      // if(!(1 == res)) handleFailure("verify locations");

      ssl=SSL_new(ctx);

      SSL_set_fd(ssl, sockfd);
      printf("joined sock with ssl\n");
      err = SSL_connect(ssl);
      if (err < 1) {printf("ssl connecting failed %d.\n", err);}
    #endif

    printf("connected\n");



    // write a simple request

    char* message = "LOGIN unifr:OPisgreat;";

    sleep(5); // wait until message is sent to give some time to the testing
               // programmer
    printf("Wrting %s\n", message);
    #if USE_SSL
      if (SSL_write(ssl, message, strlen(message)) <= 0) {
        printf("sending ssl failed.\n");
      }
    #else
      if (write(sockfd, message, strlen(message)) <0) {
        printf("Sending failed.\n");
      }
    #endif


    #if USE_SSL
      n = SSL_read(ssl, recvBuff, sizeof(recvBuff) - 1);
    #else
      n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
    #endif
    if (n> 0) recvBuff[n] = 0;
    if(fputs(recvBuff, stdout) == EOF) {
        printf("\n Error : Fputs error\n");
    }

    if(n < 0) {
        printf("\n Read error \n");
    }

    if(!strcmp(recvBuff, SUCCESS_LOGIN("unifr"))) {
      printf("All well, exiting now\n");
      close(sockfd);
      printf("Socket closed\n");
      exit(0);
    } else {
      printf("guess we got a wrong message\n");
    }

    return 0;
}
