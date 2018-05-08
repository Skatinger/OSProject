#ifndef LOGGER
#define LOGGER

#include "../project.h"
#include <stdio.h>

//#define LOG_INFO(X) printf("[%s -- INF]" X "\n"); fprintf(log_f, "[%s -- INF]" X "\n");


#define INFO 0
#define ERROR 1

/**
 * Logs the message with the given priority / importance
 * @param message the msg to log
 * @param type    importance (info, error etc see constants)
 */
void logger(char* message, int type);

void init_logger(char* name);

static char* time_string();

#endif
