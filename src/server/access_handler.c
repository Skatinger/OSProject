#include <semaphore.h>

#include "serverResponses.h"
#include "access_handler.h"
#include "keyvalue.h"
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

void init_access_handler(pthread_key_t* USERNAME_key) {
  users = initUserDB();
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

int login(char* password) {
  // Getting the username from the thread-global USERNAME value
  void* res;
  res = pthread_getspecific(*USERNAME);
  char* username = (char *) res;
  pthread_mutex_lock(&users_lock);
  if (checkCredentials(users, username, password)) {
    set_access_logged_in(users, username);
    pthread_mutex_unlock(&users_lock);
    return 0;
  } else {
    pthread_mutex_unlock(&users_lock);
    return -1;
  }
}

void logout() {
  char* username;
  username = (char*) pthread_getspecific(*USERNAME);
  pthread_mutex_lock(&users_lock);
  set_access_logged_in(users, username, FALSE);
  pthread_mutex_unlock(&users_lock);
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
  switch (type) {
    case PUT:
      n = set(kvs, key, (void*) value);
      if (n != SUCCESS) {/*allocate new kvs immediately!*/ }
      ret = SUCCESS_PUT(key, value);
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
