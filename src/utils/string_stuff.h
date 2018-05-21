#ifndef STRINGUTILS
#define STRINGUTILS

#include "../project.h"

#define SIZE 100

// COLORS
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"
#define BOLD "\x1b[1m"
#define INVERT "\u001b[7m"
#define COLOR_RESET "\x1b[0m"

#define RED_TXT(string) COLOR_RED string COLOR_RESET
#define GREEN_TXT(string) COLOR_GREEN string COLOR_RESET



char* concat(int num, ...);
char* intToString(int num);
char* concatCode(int code, int nbWords, ...);

char* getFirstParam(char* msg);
char* getSecondParam(char* msg);
char* getThirdParam(char* msg);

void get_password(char* prompt, char password[]);

#endif
