#ifndef HEADER_FILE
#define HEADER_FILE

#define MAX_CONNECTION_QUEUE 5
#define CERTIFICATE_FILE "../../crypto/YARcertificate.crt"
#define KEY_FILE "../../crypto/YARkey.key"

/**
 * Returns the descriptor for a new server socket.
 * @param  port the port to open
 * @return   socket descriptor
 */
int createServerSocket(int port);

struct sockaddr_in makeServerAddress(int port);

int s_listen(int socket_descriptor);

int s_connect(int socket);

SSL* s_connectTLS(int conncetionDescriptor);

void init_openssl();

SSL_CTX* create_context();

void configure_context(SSL_CTX* ctx);



#endif
