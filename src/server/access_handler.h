#ifndef ACCESS
#define ACCESS

#include "../project.h"
#include <pthread.h>

#define PUT 0
#define DEL 1
#define UPD 2

void init_access_handler(pthread_key_t* USERNAME_key);

char* reader(char* key);

char* writer(char* key, char* value, int action);

int login(char* password);

void end_access_handler();

#endif
