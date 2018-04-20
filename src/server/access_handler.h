#ifndef ACCESS
#define ACCESS

#include "../project.h"

#define PUT 0
#define DEL 1
#define UPD 2


char* reader(char* key);

char* writer(char* key, char* value, int action);

#endif
