#include "logger.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "string_stuff.h"


FILE* log_f;
int FILE_ONLY;

void init_logger(char* name, int file_only) {
  mkdir("log", 0777);
  char* filename = ss_concat(5, "log/", name, "_", time_string(), ".log");
  log_f = fopen(filename, "w");
  FILE_ONLY = file_only;
}


void close_logger() {
  fclose(log_f);
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
    if (time_string[i] == ':') time_string[i] = '-';
  }
  return time_string;
}


void logger(char* message, int type) {
  char* t = time_string();
  switch(type) {
    case INFO:
      if (!FILE_ONLY) printf("[%s -- INF] %s\n", t, message);
      fprintf(log_f, "[%s -- INF] %s\n", t, message);
      break;
    case LOGERROR:
      if (!FILE_ONLY) printf(RED_TXT("[%s -- ERR] %s\n"), t, message);
      fprintf(log_f, RED_TXT("[%s -- ERR] %s\n"), t, message);
      break;
    default:
      if (!FILE_ONLY) printf("[%s -- ???] %s\n", t, message);
      fprintf(log_f, "[%s -- ???] %s\n", t, message);
      break;
  }
}
