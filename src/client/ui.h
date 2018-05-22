#ifndef UI_H
#define UI_H

#define ASCII_ART "Art by Tua Xiong\n                 \\ __\n--==/////////////[})))==*\n                 / \\ '          ,|\n                    `\\`\\      //|                             ,|\n                      \\ `\\  //,/'                           -~ |\n   )             _-~~~\\  |/ / |'|                       _-~  / ,\n  ((            /' )   | \\ / /'/                    _-~   _/_-~|\n (((            ;  /`  ' )/ /''                 _ -~     _-~ ,/'\n ) ))           `~~\\   `\\/'/|'           __--~~__--\\ _-~  _/, \n((( ))            / ~~    \\ /~      __--~~  --~~  __/~  _-~ /\n ((\\~\\           |    )   | '      /        __--~~  \\-~~ _-~\n    `\\(\\    __--(   _/    |'\\     /     --~~   __--~' _-~ ~|\n     (  ((~~   __-~        \\~\\   /     ___---~~  ~~\\~~__--~ \n      ~~\\~~~~~~   `\\-~      \\~\\ /           __--~~~'~~/\n                   ;\\ __.-~  ~-/      ~~~~~__\\__---~~ _..--._\n                   ;;;;;;;;'  /      ---~~~/_.-----.-~  _.._ ~\\     \n                  ;;;;;;;'   /      ----~~/         `\\,~    `\\ \\        \n                  ;;;;'     (      ---~~/         `:::|       `\\.      \n                  |'  _      `----~~~~'      /      `:|        ()))),      \n            ______/\\/~    |                 /        /         (((((())  \n          /~;;.____/;;'  /          ___.---(   `;;;/             )))'`))\n         / //  _;______;'------~~~~~    |;;/\\    /                ((   ( \n        //  \\ \\                        /  |  \\;;,\\                 `   \n       (<_    \\ \\                    /',/-----'  _> \n        \\_|     \\_                 //~;~~~~~~~~~ \n                 \\_|               (,~~   -Tua Xiong\n                                    \\~\n                                     ~~"

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

static void delete_user();
static void update_user();
static void make_admin();

/**
 * This handles an update request by the user.
 */
static void update();

/**
 * This handles a logout request by the user. i.e. this user is logged out
 * and a login propmpt is openend.
 */
static void logout();

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
