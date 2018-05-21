#include "logger.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

void logger(char* message, int type) {
  time_t now;
  now = time(NULL);
  char* time_string = ctime(&now);
  // cutting of weekday
  time_string = &time_string[4];
  // and the newline
  time_string[strlen(time_string) - 1] = 0;
  switch(type) {
    case INFO:
      printf("[%s -- INF] %s\n", time_string, message);
      break;
    case ERROR:
      printf("[%s -- ERR] %s\n", time_string, message);
      break;
    default:
      printf("[%s -- ???] %s\n", time_string, message);
      break;
  }
}
