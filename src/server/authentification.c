#include <stdlib.h>
#include <stdio.h>
#include <nettle/sha3.h>
#include <pthread.h>
#include <string.h>
#include "authentification.h"

// int global_count;
// pthread_mutex_t* counterLock;

void initUserHandler() {
  // global_count = 0;
  //pthread_mutex_init(counterLock, NULL);
}

user_db_t* initUserDB() {
  user_db_t* db = malloc(sizeof(user_db_t));
  db->count = 0;
  db->table = malloc(MAXUSERS * sizeof(user_t));
  return db;
}

user_t* initUser() {
  user_t* user = malloc(sizeof(user_t));
  user->id = 0;
  user->username = NULL;
  user->passwordHash = malloc(SHA3_512_DIGEST_SIZE * sizeof(uint8_t));
  user->salt = malloc(SALT_LENGTH * sizeof(char));
  user->rights = 0;
  return user;
}

user_t* newUser(char* username, char* password, int rights) {
  user_t* user = initUser();

  // pthread_mutex_lock(counterLock);
  // user->id = global_count;
  // pthread_mutex_unlock(counterLock);

  // TODO: check for validity...
  user->username = username;
  user->rights = rights;

  time_t t;
  srand((unsigned) time(&t) * (unsigned int) username[0]);

  for (int i = 0; i < SALT_LENGTH; i++) {
    int r = rand() % (2 * ALPHABET_SIZE);
    r += (r < ALPHABET_SIZE) ? (int) 'a' : (int) 'A' - ALPHABET_SIZE;
    user->salt[i] = (char) r;
  }


  (*user).passwordHash = createHash(password, user->salt);
  return user;
}

uint8_t* createHash(char* password, char* salt) {
  struct sha3_512_ctx ctx;
  sha3_512_init(&ctx);

  int length = strlen(password);
  char* dest = malloc((SALT_LENGTH + length) * sizeof(char));

  strcpy(dest, password);
  strcat(dest, salt);

  //printf("%s\n", dest);

  sha3_512_update(&ctx, strlen(dest), (uint8_t*) dest);
  uint8_t* ret = calloc(SHA3_512_DIGEST_SIZE, sizeof(uint8_t));
  sha3_512_digest(&ctx, SHA3_512_DIGEST_SIZE, ret);

  return ret;
}

int checkCredentials(user_db_t* db, char* username, char* password) {
  user_t* user = getUserByName(db, username);
  if (user != NULL) {
    uint8_t* hash = createHash(password, user->salt);
    for (int k = 0; k < SHA3_512_DIGEST_SIZE; k++) {
      if (hash[k] != user->passwordHash[k]) return FALSE;
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

user_t* getUserByName(user_db_t* db, char* username) {
  // TODO: better data access
  for (int i = 0; i < db->count; i++) {
    if (!strcmp(db->table[i].username, username)) {
      return &db->table[i];
    }
  }
  return NULL;
}

int addUser(user_db_t* db, user_t* user) {
  db->count++;
  db->table[db->count-1] = *user;

  // pthread_mutex_lock(counterLock);
  // global_count++;
  // user->id = global_count;
  // pthread_mutex_unlock(counterLock);

  return 0;
}

void printUser(user_t* user) {
  char* hashString = malloc(2*SHA3_512_DIGEST_SIZE * sizeof(char));
  for (int i = 0; i < SHA3_512_DIGEST_SIZE; i++) {
    //char byte[2];
    sprintf(hashString+2*i, "%02x", user->passwordHash[i]);
  }
  printf("----------------------------\n");
  printf("USER INFO\n");
  printf("Name: %s, \nHash: %s, \nid: %d, \nsalt: %s, \nrights: %d\n", user->username, hashString, user->id, user->salt, user->rights);
  printf("----------------------------\n");

}
