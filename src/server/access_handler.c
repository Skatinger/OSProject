#include <semaphore.h>

#include "server_responses.h"
#include "access_handler.h"
#include "key_value.h"
#include "authentification.h"
#include "../utils/logger.h"
#include <string.h>

static user_db_t* users;
static KVS* kvs;
static pthread_mutex_t users_lock;
static pthread_mutex_t kvs_lock;
static pthread_mutex_t counter_lock;
static int writer_waiting;
static int writer_done;
static int reader_count;

static int STORE_SIZE = 1000;
pthread_key_t* USERNAME;

void init_access_handler(pthread_key_t* USERNAME_key, char* root_pw) {
  users = init_user_db();
  addUser(users, "root", root_pw, ADMIN);
  kvs = create(STORE_SIZE);
  pthread_mutex_init(&kvs_lock, NULL);
  pthread_mutex_init(&users_lock, NULL);
  pthread_mutex_init(&counter_lock, NULL);

  // This is the key used to get the thread-global username.
  USERNAME = USERNAME_key;

  writer_waiting = FALSE;
  writer_done = FALSE;
  reader_count = 0;
}

char* login(char* password) {
  // Getting the username from the thread-global USERNAME value
  void* res;
  res = pthread_getspecific(*USERNAME);
  char* username = (char *) res;

  pthread_mutex_lock(&users_lock);
  if (checkCredentials(users, username, password)) {
    set_access_logged_in(users, username, TRUE);
    pthread_mutex_unlock(&users_lock);
    return SUCCESS_LOGIN(username);
  } else {
    pthread_mutex_unlock(&users_lock);
    return ERROR_ACCESS_DENIED(username);
  }
}

char* logout() {
  char* username;
  username = (char*) pthread_getspecific(*USERNAME);
  pthread_mutex_lock(&users_lock);
  set_access_logged_in(users, username, FALSE);
  pthread_mutex_unlock(&users_lock);

  return SUCCESS_LOGOUT;
}

char* reader(char* key) {
  char* username;
  int rights;
  char* value;

  username = (char*) pthread_getspecific(*USERNAME);

  // if the user is not logged in, he may not read
  pthread_mutex_lock(&users_lock);
  if ((rights = get_access(users, username)) < 0) {
    pthread_mutex_unlock(&users_lock);
    return ERROR_ACCESS_DENIED(username);
  }
  pthread_mutex_unlock(&users_lock);

  // lock the reader count and increment
  pthread_mutex_lock(&counter_lock);
  reader_count++;
  // if this is the first reader or the writers are done and give back the db,
  // acquire the lock for the kvs
  if (reader_count == 1 || writer_done) {
    pthread_mutex_lock(&kvs_lock);
    writer_done = FALSE;
  }
  // unlock the counter
  pthread_mutex_unlock(&counter_lock);

  // ACTUAL KVS ACCESS
  value = (char*) get(kvs, key);

  // decrement the counter again
  pthread_mutex_lock(&counter_lock);
  reader_count--;

  // if it was the last reader or there's a writer waiting, give back the lock
  if (reader_count == 0 || writer_waiting) {
    pthread_mutex_unlock(&kvs_lock);
    writer_waiting = FALSE;
  }
  pthread_mutex_unlock(&counter_lock);
  return value != NULL ? SUCCESS_GOT(key, value) : ERROR_KEY_NOT_FOUND(key);
}

char* user_db_new(char *username, char* password){
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = addUser(users, username, password, NORMAL);
    logger("added new user\n", INFO);
    pthread_mutex_unlock(&users_lock);
    if (r == SUCCESS) {
      return SUCCESS_ADD_U(username);
    } else if (r == ERROR_USERNAME_TOO_LONG) {
      return ERROR_USERNAME_INVALID;
    } else if (r == ERROR_USERNAME_TAKEN) {
      return ERROR_USER_OCCUPIED(username);
    } else {
      return ERROR_USER_MODIFICATION;
    }

  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* user_db_delete(char* username) {
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = delete_user(users, username);
    logger("deleted user\n", INFO);
    pthread_mutex_unlock(&users_lock);
    return r == 0 ? SUCCESS_DEL_U(username) : ERROR_USER_MODIFICATION;
  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* user_db_update(char* old_username, char* new_username, char* new_password) {
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = update_user(users, old_username, new_username, new_password);
    logger("deleted user\n", INFO);
    pthread_mutex_unlock(&users_lock);
    if (r == SUCCESS) {
      return SUCCESS_CHG_U(old_username, new_username);
    } else if (r == ERROR_USERNAME_TOO_LONG) {
      return ERROR_USERNAME_INVALID;
    } else if (r == ERROR_USERNAME_TAKEN) {
      return ERROR_USER_OCCUPIED(new_username);
    } else if (r == ERROR_USER_LOGGEDIN) {
      return  ERROR_USER_NOT_UPDATABLE(new_username);
    } else {
      return ERROR_USER_MODIFICATION;
    }
  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* user_db_admin(char* username) {
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = set_access_rights(users, username, ADMIN);
    logger("deleted user\n", INFO);
    pthread_mutex_unlock(&users_lock);
    return r == SUCCESS ? SUCCESS_MK_ADM(username) : ERROR_USER_MODIFICATION;
  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* writer(char* key, char* value, int type) {
  // TODO: error stuff
  char* ret;
  char* whatever = malloc(BUFFER_SIZE * sizeof(char));
  int n;
  char* username = (char*) pthread_getspecific(*USERNAME);
  int rights;

  // Check if user actually has access
  pthread_mutex_lock(&users_lock);
  if ((rights = get_access(users, username)) < 0) {
    pthread_mutex_unlock(&users_lock);
    return ERROR_ACCESS_DENIED(username);
  }
  pthread_mutex_unlock(&users_lock);


  pthread_mutex_lock(&counter_lock);
  writer_waiting = TRUE;
  pthread_mutex_unlock(&counter_lock);
  pthread_mutex_lock(&kvs_lock);
  // TODO: Actually check for what the kvs is trying desperatley to let
  // you know
  //komment
  switch (type) {
    case PUT:
      n = set(kvs, key, (void*) value);
      if (n == KEY_IN_USE_ERROR) {
        ret = ERROR_KEY_OCCUPIED(key);
      } else if (n == STORAGE_FULL_ERROR) {
        ret = ERROR_SERVER_FULL;
      } else if (n == SUCCESS) {
        ret = SUCCESS_PUT(key, value);
      } else {
        ret = "WTF\n";
      }
      break;
    case UPD:
      n = replace(kvs, key, (void*) value);
      ret = (n==SUCCESS) ? SUCCESS_UPD(key, value) : ERROR_KEY_NOT_FOUND(key);
      break;
    case DEL:
      whatever = (char*) del(kvs, key);
      if (whatever == NULL) {
        ret = ERROR_KEY_NOT_FOUND(key);
      } else {
        ret = SUCCESS_DEL(key, whatever);
      }
      // ret = "hmmm";
      break;
  }
  pthread_mutex_lock(&counter_lock);
  writer_done = TRUE;
  pthread_mutex_unlock(&counter_lock);
  pthread_mutex_unlock(&kvs_lock);

  return ret;
}


void end_access_handler() {
  printKVS(kvs);
}
