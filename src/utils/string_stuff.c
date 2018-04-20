#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "string_stuff.h"

char* concat(int num, ...) {
  va_list vlist;
  va_start(vlist, num);
  char* dest = malloc(1024*sizeof(char));
  dest[0] = '\0';
  strcat(dest, va_arg(vlist, char*));
  for (int i = 1; i < num; i++) {
    char* source = va_arg(vlist, char*);
    strcat(dest,source);
  }
  va_end(vlist);
  return dest;
}

char* intToString(int num) {
  char* ret = malloc(4*sizeof(char));
  sprintf(ret, "%d", num);
  ret[3] = 0;
  return ret;
}

char* concatCode(int code, int nbWords, ...) {
  va_list vlist;
  va_start(vlist, nbWords);
  char* dest = malloc(1024* sizeof(char));
  dest[0] = 0;
  strcpy(dest, intToString(code));
  for (int i = 0; i < nbWords; i++) {
    char* source = va_arg(vlist, char*);
    strcat(dest,source);
  }
  va_end(vlist);
  return dest;
}

char* getFirstParam(char* msg) {
  static char param[BUFFER_SIZE];
  int i, n, k, j;
  i = 0; n = strlen(msg); j = 0;

  // skip the 'command'
  while (msg[i] != ' ') {i++;}
  i++; k = i; // k = beginning index of the parameter
  // copy the first argument (i.e. until the second is reached, or the end
  // if there is only one)
  while (msg[i] != ':' && msg[i] != ';') {
    param[i-k] = msg[i];
    i++;
    //j++;
  }
  // null - terminate --> nicest thing about C
  param[i-k] = 0;

  return param;
}

char* getSecondParam(char* msg) {
  static char param[BUFFER_SIZE];
  int i, n, k;
  i = 0; n = strlen(msg);

  // skip the command and first arg
  while (msg[i] != ':') {i++;}
  i++; k = i; // k = beginning of the second arg
  // loop until end or third arg begins
  while (msg[i] != ';' && msg[i] != ':') {param[i-k] = msg[i]; i++;}
  param[i-k] = 0;
  return param;
}

char* getThirdParam(char* msg) {
  static char param[BUFFER_SIZE];
  int i, n, k;
  i = 0; n = strlen(msg);

  while(msg[i] != ':') {i++;}
  i++; // loop over the secon arg as well
  while(msg[i] != ':') {i++;}
  i++; k = i;
  while (msg[i] != ';') {param[i-k] = msg[i]; i++;}
  param[i-k] = 0;
  return param;
}

void get_password(char* prompt, char password[]) {
    static struct termios oldt, newt;
    int i = 0;
    int c;

    printf("%s", prompt);
    /*saving the old settings of STDIN_FILENO and copy settings for resetting*/
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;

    /*setting the approriate bit in the termios struct*/
    newt.c_lflag &= ~(ECHO);

    /*setting the new bits*/
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    /*reading the password from the console*/
    while ((c = getchar())!= '\n' && c != EOF && i < SIZE){
        password[i++] = c;
    }
    password[i] = '\0';

    /*resetting our old STDIN_FILENO*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

    return;
}
