#ifndef HEADER_FILE
#define HEADER_FILE

#include "../project.h"
#include <openssl/ssl.h>

#define MAX_CONNECTION_QUEUE 5
#define ERR_BUF_SIZE 256

/* Depends on current working dir! */
#define CERTIFICATE_FILE "crypto/YARcertificate.pem"
#define KEY_FILE "crypto/YARkey.key"
#define CA_CERT_FILE "crypto/Fake_CA/ca.crt"
#define CA_KEY_FILE "crypto/Fake_CA/key.crt"



typedef struct connection_t {
  int data_length;                // length of read data
  char buffer[BUFFER_SIZE];   // buffer to read data from this connection
  int socket_descriptor; // the socket for this connection (represented by an int)
  #if USE_TLS == TRUE
    SSL* TLS_descriptor;   // the tls connection
  #endif
} connection_t;

/**
 * Returns the descriptor for a new server socket (Basically an ID for this
 * socket).
 * @param  port the port to open
 * @return   socket descriptor
 */
static int s_create_socket(int port);

/**
 * Make a struct containing the server address (local address,
 * port) etc.
 * @param  port the port to pen
 * @return      the struct containing the address
 */
static struct sockaddr_in s_init_address(int port);

/**
 * Abstraction of the listen primitive. Has the server listen
 * to new connections.
 * Prints error message if fails.
 * @param  socket_descriptor the int identifying a socket
 * @return   0 in case of success
 */
static int s_listen(int socket_descriptor);

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
 * @param  con_info a struct describing the TCP connection
 * @return 0 if successful, other int if not.
 */
int s_read(connection_t* con_info);

/**
 * Writes to the connection descibed in the argument.
 * @param  con_info the struct with TCP connection info
 * @param message: the message to write.
 * @return      0 upon success
 */
int s_write(connection_t* con_info, char message[BUFFER_SIZE]);

// =================================================
// ============ TLS STUFF FOR future ===============
// =================================================

#if USE_TLS == TRUE
  /**
   * initialise everything needed for a TLS connection.
   */
  void s_init_TLS();

  /**
   * Abstraction of accepting a connection via TLS. similar to s_connect. Sets
   * up the context of an TLS connection and connects.
   * Handles errors ()
   * @param  connection_descriptor descriptor of a regular TCP socket connection as
   *                               created by s_connect().
   * @return                       0 if successful
   */
  //SSL* s_connect_TLS(int connection_descriptor, SSL_CTX* ctx);

  connection_t* s_connect_TLS();
  /**
   * Reads from the (TLS) connection given in the argument and stores the read STUFF
   * in that buffer. Handles errors (basic).
   * Blocks as long as there is nothing to read.
   * @param  con_info a struct describing the TCP / TLS connection
   * @return 0 if successful, other int if not.
   */
  int s_read_TLS(connection_t* con_info);

  /**
   * Writes to the TLS connection given in the argument. Handles errors.
   * @param  con_info    [description]
   * @param  message [description]
   * @return         [description]
   */
  int s_write_TLS(connection_t* con_info, char message[BUFFER_SIZE]);

  /**
   * Ends a TLS connection.
   * @param con_info the connection to end.
   */
  void s_end_TLS(connection_t* con_info);

  /**
   * Auxiliary function used to set up a TLS connection.
   * Creates and configures the needed context for a TLS connection based on the
   * given data.
   * The context will then be used to create a new TLS connection.
   * @return the context
   */
  static SSL_CTX* s_create_TLS_context();

  /**
   * Used to print TLS errors. Exits the program if the arg is nonzero.
   * @param error_msg A message to print about the error
   * @param indicates if the program should be stopped
   */
  static void s_TLS_error(char* error_msg, int exit_program);

#endif


#endif
