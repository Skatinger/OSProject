#include "logger.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "string_stuff.h"


FILE* log_f;

void init_logger(char* name) {
  mkdir("log", 0777);
  char* filename = concat(5, "log/", name, "_", time_string(), ".log");
  log_f = fopen(filename, "w");
}

static char* time_string() {
  time_t now;
  now = time(NULL);
  char* time_string = ctime(&now);
  // cutting of weekday
  time_string = &time_string[4];
  // and the newline
  time_string[strlen(time_string) - 1] = 0;
  for (int i = 0; i < strlen(time_string); i++) {
    if (time_string[i] == ' ') time_string[i] = '_';
  }
  return time_string;
}


void logger(char* message, int type) {
  switch(type) {
    case INFO:
      printf("[%s -- INF] %s\n", time_string(), message);
      break;
    case ERROR:
      printf(RED_TXT("[%s -- ERR] %s\n"), time_string(), message);
      break;
    default:
      printf("[%s -- ???] %s\n", time_string(), message);
      break;
  }
}
