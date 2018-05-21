#include <semaphore.h>

#include "serverResponses.h"
#include "access_handler.h"
#include "keyvalue.h"
#include "authentification.h"
#include "../utils/logger.h"
#include <string.h>

static KVS* access_rights;
static KVS* kvs;
static pthread_mutex_t access_rights_lock;
static pthread_mutex_t kvs_lock;
static pthread_mutex_t counter_lock;
static int writer_waiting;
static int writer_done;
static int reader_count;

static int STORE_SIZE = 1000;
pthread_key_t* USERNAME;

void init_access_handler(pthread_key_t* USERNAME_key) {
  access_rights = create(MAX_CONNECTIONS);
  kvs = create(STORE_SIZE);
  pthread_mutex_init(&kvs_lock, NULL);
  pthread_mutex_init(&access_rights_lock, NULL);
  pthread_mutex_init(&counter_lock, NULL);
  USERNAME = USERNAME_key;
  writer_waiting = FALSE;
  writer_done = FALSE;
  reader_count = 0;
  //logger("adding a test kvp (bla)\n", INFO);
  //set(kvs, "bla", (void*) "this is a test message. If you see it, very nice");
}

int login(char* password) {
  void* res;
  res = pthread_getspecific(*USERNAME);
  char* username = (char *) res;
  //printf("global username: %s", username);
  user_t* user = getUserByName(username);
  if (checkCredentials(username, password)) {
    int rights;
    if (user != NULL) rights = user->rights;
    pthread_mutex_lock(&access_rights_lock);
    set(access_rights, username, (void*)rights);
    pthread_mutex_unlock(&access_rights_lock);
    return 0;
  } else {
    pthread_mutex_lock(&access_rights_lock);
    set(access_rights, username, (void*)-1);
    pthread_mutex_unlock(&access_rights_lock);
    return -1;
  }
}

char* reader(char* key) {
  char* value;
  pthread_mutex_lock(&counter_lock);
  reader_count++;
  if (reader_count == 1 || writer_done) {
    pthread_mutex_lock(&kvs_lock);
    writer_done = FALSE;
  }
  pthread_mutex_unlock(&counter_lock);
  value = (char*) get(kvs, key);
  pthread_mutex_lock(&counter_lock);
  reader_count--;
  if (reader_count == 0 || writer_waiting) {
    pthread_mutex_unlock(&kvs_lock);
    writer_waiting = FALSE;
  }
  pthread_mutex_unlock(&counter_lock);
  return value != NULL ? SUCCESS_GOT(key, value) : ERROR_KEY_NOT_FOUND(key);
}

//TODO implement level of access. atm everyone gets root access
char* user_db_writer(char *username, char* password){
  user_t* user = newUser(username, password, ADMIN);
  addUser(user);
  logger("added new user\n", INFO);
  return SUCCESS_ADD_U(user);
}

char* writer(char* key, char* value, int type) {
  // TODO: error stuff
  char* ret;
  char* whatever = malloc(BUFFER_SIZE * sizeof(char));
  int n;
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
