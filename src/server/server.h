#ifndef HEADER_FILE
#define HEADER_FILE

#include "../project.h"

#define MAX_CONNECTION_QUEUE 5
#define ERR_BUF_SIZE 256

/* Depends on current working dir! */
#define CERTIFICATE_FILE "crypto/YARcertificate.pem"
#define KEY_FILE "crypto/YARkey.key"
#define CA_CERT_FILE "crypto/Fake_CA/ca.crt"
#define CA_KEY_FILE "crypto/Fake_CA/key.crt"



#include "connectionHandler.h"

/**
 * Returns the descriptor for a new server socket.
 * @param  port the port to open
 * @return   socket descriptor
 */
int createServerSocket(int port);

/**
 * Make a struct containing the server address (local address,
 * port) etc.
 * @param  port the port to pen
 * @return      the struct containng the address
 */
struct sockaddr_in makeServerAddress(int port);

/**
 * Abstraction of the listen primitive. Has the server listen
 * to new connections.
 * Prints error message if fails.
 * @param  socket_descriptor the int identifying a socket
 * @return   0 in case of success
 */
int s_listen(int socket_descriptor);

/**
 * Abstraction of the accept primitive -- blocks the thread until
 * a connection request of a client arrives. Once that happens,
 * the connection is started.
 * @param  socket the descriptor of the socket which should connect
 * @return  descriptor of the newly created connection to the client
 *          (basically a new socket)
 */
int s_connect(int socket);

/**
 * Reads from the connection given in the argument and stores the read STUFF
 * int that buffer. Handles errors (basic).
 * Blocks as long as there is nothing to read.
 * @param  cinf a struct describing the TCP connection
 * @return 0 if successful, other int if not.
 */
int s_read(connectionInfo* cinf);

/**
 * Writes to the connection descibed in the argument.
 * @param  cinf the struct with TCP connection info
 * @param message: the message to write.
 * @return      0 upon success
 */
int s_write(connectionInfo* cinf, char message[BUFFERSIZE]);

// =================================================
// ============ TLS STUFF FOR future ===============
// =================================================

#if USE_SSL == TRUE
/**
 * Abstraction of accepting a connection via TLS. similar to s_connect.
 * @param  connection_descriptor descriptor of a regular TCP socket connection as
 *                               created by s_connect().
 * @param  ctx                   the context of the TSL connection.
 * @return                       a new TLS connection data struct.
 */
//SSL* s_connectTLS(int connection_descriptor, SSL_CTX* ctx);

int s_connectTLS(int connection_descriptor, SSL_CTX* ctx, SSL** ssl_to_store);

/**
 * Reads from the (TLS) connection given in the argument and stores the read STUFF
 * in that buffer. Handles errors (basic).
 * Blocks as long as there is nothing to read.
 * @param  cinf a struct describing the TCP / TLS connection
 * @return 0 if successful, other int if not.
 */
int s_readTLS(connectionInfo* cinf);

int s_writeTLS(connectionInfo* cinf, char message[BUFFERSIZE]);

/**
 * initialise everything needed for a TLS connection.
 */
void init_openssl();

/**
 * Creates and configures the needed context for a TLS connection based on the given data.
 * @return the context
 */
SSL_CTX* create_context();
#endif


#endif
