#ifndef ACCESS
#define ACCESS

#include "../project.h"
#include <pthread.h>

#define PUT 0
#define DEL 1
#define UPD 2

#define GET 0
#define KEY 1

void ah_init_access_handler(pthread_key_t* USERNAME_key, char* root_pw);

char* ah_get(char* key);

char* ah_reader(char* key, int type);

char* ah_writer(char* key, char* value, int action);

char* ah_keys(char* value);

char* ah_user_db_new(char *username, char* password);

char* ah_login(char* password);

char* ah_logout();

char* ah_user_db_delete(char* username);

char* ah_user_db_update(char* old_username, char* new_username, char* new_password);

char* ah_user_db_admin(char* username);

void ah_end_access_handler();

#endif
