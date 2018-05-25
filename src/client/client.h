#ifndef HEADER_FILE
#define HEADER_FILE

#include "../project.h"
#include <openssl/ssl.h>

#define ERR_BUF_SIZE 256

// ========== "PUBLIC" ==============

/**
 * Connects the client socket to the server using a secure TLS connection.
 * Hides the underlying socket and everything.
 * Handles errors.
 * @param  ip_address IP of the server to connect to
 * @return        0 on success
 */
int c_connect_TLS(char* ip_address);

/**
 * Used to write a message to the underlying TLS connection (i.e. send a message
 * to the server).
 * Handles errors.
 * @param  message a string with the message
 * @return         0 on success, anything else on failure.
 */
int c_send_TLS(char message[BUFFER_SIZE]);

/**
 * Reads from the TLS connection (i.e. receives messages from the server) and
 * stores them in the buffer.
 * Handles errors.
 * @param  buffer a buffer to store the rcved msg in
 * @return        0 on success, anything else on failure
 */
int c_receive_TLS(char buffer[BUFFER_SIZE]);

/**
 * Ends the connection.
 */
void c_end_TLS();

// =========== "PRIVATE" ============

/**
 * Returns the descriptor for a new client socket (Basically an ID for this
 * socket).
 * @return   socket descriptor
 */
static int c_create_socket();

/**
 * Takes an IP Address as a string (e.g. "127.0.0.1") and turns it into a
 * usable struct.
 * @param  port       the port
 * @param  ip_address ip address (as string!)
 * @return            the struct
 */
static struct sockaddr_in c_parse_address(int port, char* ip_address);

/**
 * Initialises everything needed for the secure TLS connection, in particular
 * the library and the socket needed.
 */
static void c_init_TLS();

/**
 * Creates the context for the TLS connection and configures it accordingly.
 * @return the context
 */
static SSL_CTX* c_create_context();

/**
 * Used to print TLS errors. Exits the program if the arg is nonzero.
 * @param error_msg A message to print about the error
 * @param indicates if the program should be stopped
 */
static void c_TLS_error(char* error_msg, int exit_program);

#endif
