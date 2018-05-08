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
  logger("adding a test kvp (bla)\n", INFO);
  set(kvs, "bla", (void*) "this is a test message. If you see it, very nice");
}

int login(char* password) {
  void* res;
  res = pthread_getspecific(*USERNAME);
  char* username = (char *) res;
  printf("global username: %s\n", username);
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

void logout() {
  char* username;
  username = pthread_getspecific(*USERNAME);
  pthread_mutex_lock(&access_rights_lock);
  del(access_rights, username);
  pthread_mutex_unlock(&access_rights_lock);
  print_access_rights();
}

char* reader(char* key) {
  int rights;
  char* value;
  if ((rights = get_rights()) < 0) return NULL;

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

char* writer(char* key, char* value, int type) {
  // TODO: error stuff
  char* ret;
  char* whatever = malloc(BUFFER_SIZE * sizeof(char));
  int n;
  int rights;
  if ((rights = get_rights()) < 0) return NULL;
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
  printf("global: %s\n", pthread_getspecific(*USERNAME));
  return ret;
}

static int get_rights() {
  int r;
  char* username;
  username = pthread_getspecific(*USERNAME);
  pthread_mutex_lock(&access_rights_lock);
  r = (int) get(access_rights, username);
  pthread_mutex_unlock(&access_rights_lock);
  return r;
}

static void print_access_rights() {
  for (int i = 0; i < access_rights->size; i++) {
    if (access_rights->table[i].key == NULL) continue;
    printf("i = %d : key = %s value = %d \n", i, access_rights->table[i].key, (int) access_rights->table[i].value);
  }
}

void end_access_handler() {
  printKVS(kvs);
  printf("now printing access rights\n");
  print_access_rights();
  printf("Get global var: %s\n", pthread_getspecific(*USERNAME));
}
