#include <semaphore.h>

#include "server_responses.h"
#include "access_handler.h"
#include "key_value_v3.h"
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

void ah_init_access_handler(pthread_key_t* USERNAME_key, char* root_pw) {
  users = u_init_user_db();
  u_add_user(users, "root", root_pw, ADMIN);
  kvs = kvs_create(STORE_SIZE);
  pthread_mutex_init(&kvs_lock, NULL);
  pthread_mutex_init(&users_lock, NULL);
  pthread_mutex_init(&counter_lock, NULL);

  // This is the key used to get the thread-global username.
  USERNAME = USERNAME_key;

  writer_waiting = FALSE;
  writer_done = FALSE;
  reader_count = 0;
}

char* ah_login(char* password) {
  // Getting the username from the thread-global USERNAME value
  void* res;
  res = pthread_getspecific(*USERNAME);
  char* username = (char *) res;

  logger("Acquiring user lock", INFO);
  pthread_mutex_lock(&users_lock);
  if (u_get_logged_in(users, username)) {
    logger(ss_concat(2, username, "already logged in"), INFO);
    pthread_mutex_unlock(&users_lock);
    logger("User lock unlocked", INFO);
    return ERROR_ALREADY_LOGGEDIN(username);
  }
  if (u_check_credentials(users, username, password)) {
    logger(ss_concat(2, "Setting logged_in to TRUE for ", username), INFO);
    pthread_t* this_thread = malloc(sizeof(pthread_t));
    // *this_thread = pthread_self();
    // logger(ss_concat(2, "Current thread is ", ss_int_to_3digit_string(*this_thread), INFO);
    u_set_access_logged_in(users, username, TRUE);
    pthread_mutex_unlock(&users_lock);
    logger("User lock unlocked", INFO);
    return SUCCESS_LOGIN(username);
  } else {
    pthread_mutex_unlock(&users_lock);
    logger("user lock unlocked", INFO);
    return ERROR_ACCESS_DENIED(username);
  }
}

char* ah_logout() {
  char* username;
  username = (char*) pthread_getspecific(*USERNAME);
  pthread_mutex_lock(&users_lock);

  // If the caller doesn't actually have access,
  // they should not be allowed to log out, hence return
  if (u_get_access(users, username) < NORMAL) {
    pthread_mutex_unlock(&users_lock);
    // TODO: perhaps give error message here.
    return SUCCESS_LOGOUT;
  }
  u_set_access_logged_in(users, username, FALSE);
  pthread_mutex_unlock(&users_lock);

  return SUCCESS_LOGOUT;
}

char* ah_reader(char* input, int TYPE) {
  char* username;
  int rights;
  char* value;

  username = (char*) pthread_getspecific(*USERNAME);

  // if the user is not logged in, he may not read
  pthread_mutex_lock(&users_lock);
  if ((rights = u_get_access(users, username)) < 0) {
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

  char* reply;
  // ACTUAL KVS ACCESS
  switch(TYPE) {
    case GET:
      value = (char*) kvs_get(kvs, input);
      reply = value != NULL ? SUCCESS_GOT(input, value) : ERROR_KEY_NOT_FOUND(input);
      break;
    case KEY:
      value =  kvs_keys_for_string_value(kvs, input);
      reply = value != NULL ? SUCCESS_KEY(value) : ERROR_VALUE_INVALID;
      break;
  }


  // decrement the counter again
  pthread_mutex_lock(&counter_lock);
  reader_count--;

  // if it was the last reader or there's a writer waiting, give back the lock
  if (reader_count == 0 || writer_waiting) {
    pthread_mutex_unlock(&kvs_lock);
    writer_waiting = FALSE;
  }
  pthread_mutex_unlock(&counter_lock);


  return reply;
}

char* ah_user_db_new(char *username, char* password){
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (u_get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = u_add_user(users, username, password, NORMAL);
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

char* ah_user_db_delete(char* username) {
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (u_get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = u_delete_user(users, username);
    logger("deleted user\n", INFO);
    pthread_mutex_unlock(&users_lock);
    return r == 0 ? SUCCESS_DEL_U(username) : ERROR_USER_MODIFICATION;
  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* ah_user_db_update(char* old_username, char* new_username, char* new_password) {
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (u_get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = u_update_user(users, old_username, new_username, new_password);
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

char* ah_user_db_admin(char* username) {
  char* current_user;
  current_user = (char*) pthread_getspecific(*USERNAME);

  if (u_get_access(users, current_user) >= ADMIN) {
    pthread_mutex_lock(&users_lock);
    int r = u_set_access_rights(users, username, ADMIN);
    logger("deleted user\n", INFO);
    pthread_mutex_unlock(&users_lock);
    return r == SUCCESS ? SUCCESS_MK_ADM(username) : ERROR_USER_MODIFICATION;
  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* ah_writer(char* key, char* value, int type) {
  // TODO: error stuff
  char* ret;
  char* whatever = malloc(BUFFER_SIZE * sizeof(char));
  int n;
  char* username = (char*) pthread_getspecific(*USERNAME);
  int rights;

  // Check if user actually has access
  pthread_mutex_lock(&users_lock);
  if ((rights = u_get_access(users, username)) < 0) {
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
      n = kvs_set(kvs, key, (void*) value);
      if (n == KEY_TAKEN) {
        ret = ERROR_KEY_OCCUPIED(key);
      } else if (n == OVERFLOW) {
        ret = ERROR_SERVER_FULL;
      } else if (n == SUCCESS) {
        ret = SUCCESS_PUT(key, value);
      } else {
        ret = "WTF\n";
      }
      break;
    case UPD:
      n = kvs_replace(kvs, key, (void*) value);
      ret = (n==SUCCESS) ? SUCCESS_UPD(key, value) : ERROR_KEY_NOT_FOUND(key);
      break;
    case DEL:
      whatever = (char*) kvs_del(kvs, key);
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


void ah_end_access_handler() {
  print_kvs(kvs);
}
