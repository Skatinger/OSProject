#ifndef LOGGER
#define LOGGER

#include "../project.h"

#define INFO 0
#define ERROR 1

/**
 * Logs the message with the given priority / importance
 * @param message the msg to log (w/o newline)
 * @param type    importance (info, error etc see constants)
 */
void logger(char* message, int type);

#endif
