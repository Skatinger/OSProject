#include <semaphore.h>

#include "server_responses.h"
#include "access_handler.h"
#include "key_value_v3.h"
#include "authentication.h"
#include "../utils/logger.h"
#include <string.h>

static user_db_t* users;
static KVS* kvs;
int writer_waiting;
int writer_done;
int reader_count;

static int STORE_SIZE = 1000;
pthread_key_t* USERNAME;


void ah_init_access_handler(pthread_key_t* USERNAME_key, char* root_pw) {
  users = u_init_user_db();
  u_add_user(users, "root", root_pw, ADMIN);
  kvs = kvs_create(STORE_SIZE);


  // This is the key used to get the thread-global username.
  USERNAME = USERNAME_key;


}

char* ah_login(char* password) {
  // Getting the username from the thread-global USERNAME value
  void* res;
  res = pthread_getspecific(*USERNAME);
  char* username = (char *) res;
  int logged_in;
  int credentials;
  Node* user_dest = kvs_find_node(users->store, username);

  pthread_mutex_lock(&user_dest->lock);
  logged_in = u_get_logged_in(users, username);
  pthread_mutex_unlock(&user_dest->lock);
  if (logged_in) {
    logger(ss_concat(2, username, "already logged in"), INFO);
    return ERROR_ALREADY_LOGGEDIN(username);
  }

  pthread_mutex_lock(&user_dest->lock);
  credentials = u_check_credentials(users, username, password);
  pthread_mutex_unlock(&user_dest->lock);

  if (credentials) {
    logger(ss_concat(2, "Setting logged_in to TRUE for ", username), INFO);

    pthread_mutex_lock(&user_dest->lock);
    u_set_access_logged_in(users, username, TRUE);
    pthread_mutex_unlock(&user_dest->lock);
    logger("User lock unlocked", INFO);

    return SUCCESS_LOGIN(username);
  } else {
    logger("user lock unlocked", INFO);
    return ERROR_ACCESS_DENIED(username);
  }
}

char* ah_logout() {
  char* username;
  username = (char*) pthread_getspecific(*USERNAME);
  Node* node = kvs_find_node(users->store, username);
  pthread_mutex_lock(&node->lock);

  // If the caller doesn't actually have access,
  // they should not be allowed to log out, hence return
  if (u_get_access(users, username) < NORMAL) {
    pthread_mutex_unlock(&node->lock);
    return SUCCESS_LOGOUT;
  }
  u_set_access_logged_in(users, username, FALSE);
  pthread_mutex_unlock(&node->lock);

  return SUCCESS_LOGOUT;
}

char* ah_keys(char* value) {
  char* username;
  int rights;
  char* keys;
  Node* user_dest;

  username = (char*) pthread_getspecific(*USERNAME);
  user_dest = kvs_find_node(users->store, username);
  // if the user is not logged in, they may not read
  pthread_mutex_lock(&user_dest->lock);
  rights = u_get_access(users, username);
  pthread_mutex_unlock(&user_dest->lock);
  if (rights < 0) {
    return ERROR_ACCESS_DENIED(username);
  }
  pthread_mutex_unlock(&user_dest->lock);

  for (Node* i = kvs->key_nodes->first; i != NULL; i = i->next) {
    i->reader_count++;

    // if this is the first reader or the writers are done and give back the db,
    // acquire the lock for the kvs
    if (i->reader_count == 1 || i->writer_done) {
      pthread_mutex_lock(&i->lock);
      i->writer_done = FALSE;
    }
  }

  char* reply;
  keys = (char*) kvs_keys_for_string_value(kvs, value);

  reply = value != NULL ? SUCCESS_KEY(keys) : ERROR_VALUE_INVALID;

  for (Node* i = kvs->key_nodes->first; i != NULL; i = i->next) {
    i->reader_count--;
    // if it was the last reader or there's a writer waiting, give back the lock
    if (i->reader_count == 0 || i->writer_waiting) {
      pthread_mutex_unlock(&i->lock);
      i->writer_waiting = FALSE;
    }
  }

  return reply;
}

char* ah_get(char* key) {
  char* username;
  int access;
  char* value;
  Node* destination;
  Node* user_dest;

  username = (char*) pthread_getspecific(*USERNAME);

  user_dest = kvs_find_node(users->store, username);
  // if the user is not logged in, they may not read
  pthread_mutex_lock(&user_dest->lock);
  access = u_get_access(users, username);
  pthread_mutex_unlock(&user_dest->lock);
  if (access < 0) {
    return ERROR_ACCESS_DENIED(username);
  }
  // lock the reader count and increment
  //pthread_mutex_lock(&counter_lock);
  //
  destination = kvs_find_node(kvs, key);
  destination->reader_count++;

  // if this is the first reader or the writers are done and give back the db,
  // acquire the lock for the kvs
  if (destination->reader_count == 1 || destination->writer_done) {
    pthread_mutex_lock(&destination->lock);
    destination->writer_done = FALSE;
  }
  // unlock the counter
  //pthread_mutex_unlock(&counter_lock);

  char* reply;
  value = (char*) kvs_get(kvs, key);

  reply = value != NULL ? SUCCESS_GOT(key, value) : ERROR_KEY_NOT_FOUND(key);


  // decrement the counter again
  //(&counter_lock);
  destination->reader_count--;

  // if it was the last reader or there's a writer waiting, give back the lock
  if (destination->reader_count == 0 || destination->writer_waiting) {
    pthread_mutex_unlock(&destination->lock);
    destination->writer_waiting = FALSE;
  }
  //pthread_mutex_unlock(&counter_lock);


  return reply;
}

char* ah_user_db_new(char *username, char* password){
  char* current_user;
  int access;
  current_user = (char*) pthread_getspecific(*USERNAME);

  Node* current_dest = kvs_find_node(users->store, current_user);
  pthread_mutex_lock(&current_dest->lock);
  access = u_get_access(users, current_user);
  pthread_mutex_unlock(&current_dest->lock);

  Node* new_dest = kvs_find_node(users->store, username);

  if (access >= ADMIN) {
    pthread_mutex_lock(&new_dest->lock);
    int r = u_add_user(users, username, password, NORMAL);
    logger("added new user\n", INFO);
    pthread_mutex_unlock(&new_dest->lock);
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
  int access;
  int is_logged_in;
  current_user = (char*) pthread_getspecific(*USERNAME);
  Node* current_dest = kvs_find_node(users->store, current_user);
  pthread_mutex_lock(&current_dest->lock);
  access = u_get_access(users, current_user);
  pthread_mutex_unlock(&current_dest->lock);

  // check if the user to be deleted is currently logged in.
  Node* destination = kvs_find_node(users->store, username);


  if (access >= ADMIN ) {
    pthread_mutex_lock(&destination->lock);
    int r = u_delete_user(users, username);
    pthread_mutex_unlock(&destination->lock);

    logger("deleted user\n", INFO);
    if (r == SUCCESS) {
      return SUCCESS_DEL_U(username);
    } else if (r == ERROR_USER_LOGGEDIN) {
      return  ERROR_USER_NOT_UPDATABLE(username);
    } else {
      return ERROR_USER_MODIFICATION;
    }
  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* ah_user_db_update(char* old_username, char* new_username, char* new_password) {
  char* current_user;
  Node* current_dest;
  Node* new_dest;
  int access;

  current_user = (char*) pthread_getspecific(*USERNAME);
  current_dest = kvs_find_node(users->store, current_user);

  new_dest = kvs_find_node(users->store, new_username);
  pthread_mutex_lock(&current_dest->lock);
  access = u_get_access(users, current_user);
  pthread_mutex_unlock(&current_dest->lock);

  if (access >= ADMIN) {
    if (new_dest != current_dest) {
      // use a trylock here to avoid deadlock (if at the same time) the other
      // node tries to lock both
      int e = pthread_mutex_trylock(&new_dest->lock);
      if (e != 0) {
        // no lock can be obtained
        return ERROR_USER_MODIFICATION;
      }
    }
    int r = u_update_user(users, old_username, new_username, new_password);
    pthread_mutex_unlock(&current_dest->lock);
    if (new_dest != current_dest) {
      pthread_mutex_unlock(&new_dest->lock);
    }
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
  int access;
  current_user = (char*) pthread_getspecific(*USERNAME);
  Node* current_dest = kvs_find_node(users->store, current_user);
  pthread_mutex_lock(&current_dest->lock);
  access = u_get_access(users, current_user);
  pthread_mutex_unlock(&current_dest->lock);

  Node* dest = kvs_find_node(users->store, username);
  if (access >= ADMIN) {
    pthread_mutex_lock(&dest->lock);
    int r = u_set_access_rights(users, username, ADMIN);
    pthread_mutex_unlock(&dest->lock);
    return r == SUCCESS ? SUCCESS_MK_ADM(username) : ERROR_USER_MODIFICATION;
  } else {
    return ERROR_NO_ADMIN(current_user);
  }
}

char* ah_writer(char* key, char* value, int type) {
  char* ret;
  char* whatever = malloc(BUFFER_SIZE * sizeof(char));
  int n;
  char* username = (char*) pthread_getspecific(*USERNAME);
  int access;
  Node* destination, *user_dest;
  destination = kvs_find_node(kvs, key);
  user_dest = kvs_find_node(users->store, username);

  // Check if user actually has access
  printf("getting user lock\n");
  pthread_mutex_lock(&user_dest->lock);
  access = u_get_access(users, username);
  printf("ungetting user lock\n");
  pthread_mutex_unlock(&user_dest->lock);
  if (access < 0) {

    return ERROR_ACCESS_DENIED(username);
  }
  //pthread_mutex_unlock(&user_dest->lock);


  //pthread_mutex_lock(&counter_lock);
  destination->writer_waiting = TRUE;
  //pthread_mutex_unlock(&counter_lock);
  //
  pthread_mutex_lock(&destination->lock);

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
  //pthread_mutex_lock(&counter_lock);
  destination->writer_done = TRUE;
  //pthread_mutex_unlock(&counter_lock);
  pthread_mutex_unlock(&destination->lock);

  return ret;
}


void ah_end_access_handler() {
  print_kvs(kvs);
}
