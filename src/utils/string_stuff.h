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



char* ss_concat(int num, ...);
char* ss_int_to_3digit_string(int num);
char* ss_itoa(int num, int length);
char* ss_concat_code(int code, int nbWords, ...);

char* ss_get_first_param(char* msg);
char* ss_get_second_param(char* msg);
char* ss_get_third_param(char* msg);

void ss_get_password(char* prompt, char password[]);

#endif
