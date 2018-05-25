#ifndef ACCESS
#define ACCESS

#include "../project.h"
#include <pthread.h>

#define PUT 0
#define DEL 1
#define UPD 2

#define GET 0
#define KEY 1

void init_access_handler(pthread_key_t* USERNAME_key, char* root_pw);

char* reader(char* key, int type);

char* writer(char* key, char* value, int action);

char* user_db_new(char *username, char* password);

char* login(char* password);

char* logout();

char* user_db_delete(char* username);

char* user_db_update(char* old_username, char* new_username, char* new_password);

char* user_db_admin(char* username);

void end_access_handler();

#endif
