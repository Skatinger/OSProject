#ifndef UI_H
#define UI_H

/**
 * Hadles a login request by the user.
 * (I.e. sends the server a corresponding )
 */
static void login(void);

/**
 * This handles a get request by the user.
 */
static void get();

/**
 * This handles a quit request by the user.
 */
static void quit();

/**
 * This handles a help request by the user.
 */
static void help();

/**
 * This handles a put request by the user.
 *
 */
static void put();

/**
 * This handles a delete request by the user.
 */
static void delete();

/**
 * This handles an add user request by the user.
 */
static void add_user();

/**
 * This handles an update request by the user.
 */
static void update();

/**
 * This reads the message the server has sent and reads the code of it,
 * i.e. the first number in the string.
 * @return the integer code
 */
static int get_response_nr();

/**
 * This can be used if the server replies with an error. Puts a client-
 * readable error message to stdout.
 */
static void print_error_message();

#include "../project.h"
#endif
