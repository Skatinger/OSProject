#ifndef HEADER_FILE
#define HEADER_FILE

#include "../project.h"

/**
 * Returns the descriptor for a new client socket (Basically an ID for this
 * socket).
 * @param  port the port to open
 * @return   socket descriptor
 */
int c_create_socket(int port);

/**
 * Takes an IP Address as a string (e.g. "127.0.0.1") and turns it into a
 * usable struct.
 * @param  port       the port
 * @param  ip_address ip address (as string!)
 * @return            the struct
 */
struct sockaddr_in c_parse_address(int port, char* ip_address);

/**
 * Initialises everything needed for the secure TLS connection, in particular
 * the library and the socket needed.
 */
void c_init_TLS();

/**
 * Connects the client socket to the server using a secure TLS connection.
 * Hides the underlying socket and everything.
 * Handles errors.
 * @param  socket_descriptor [description]
 * @return                   [description]
 */
int c_connect_TLS(int socket_descriptor);

/**
 * Used to write a message to the underlying TLS connection (i.e. send a message
 * to the server).
 * Handles errors.
 * @param  message a string with the message
 * @return         0 on success, anything else on failure.
 */
int c_write_TLS(char message[BUFFER_SIZE]);

/**
 * Reads from the TLS connection (i.e. receives messages from the server) and
 * stores them in the buffer.
 * Handles errors.
 * @param  buffer a buffer to store the rcved msg in
 * @return        0 on success, anything else on failure
 */
int c_read_TLS(char buffer[BUFFER_SIZE]);


#endif
