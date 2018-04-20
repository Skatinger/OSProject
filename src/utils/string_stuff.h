#ifndef STRINGUTILS
#define STRINGUTILS

#include "../project.h"

char* concat(int num, ...);
char* intToString(int num);
char* concatCode(int code, int nbWords, ...);

char* getFirstParam(char* msg);
char* getSecondParam(char* msg);
char* getThirdParam(char* msg);


#endif
