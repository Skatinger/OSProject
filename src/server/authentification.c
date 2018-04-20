#include <stdlib.h>
#include <stdio.h>
//#include <nettle/sha3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <pthread.h>
#include <string.h>
#include "authentification.h"


int ITERATION_COUNT = 100000;
user_db_t* db;
// int global_count;
// pthread_mutex_t* counterLock;

void initUserHandler() {
  // global_count = 0;
  //pthread_mutex_init(counterLock, NULL);
}

user_db_t* initUserDB() {
  db = malloc(sizeof(user_db_t));
  db->count = 0;
  db->table = malloc(MAXUSERS * sizeof(user_t));
  return db;
}

user_t* initUser() {
  user_t* user = malloc(sizeof(user_t));
  user->id = 0;
  user->username = NULL;
  user->passwordHash = malloc(SHA_512_DIGEST_SIZE * sizeof(uint8_t));
  user->salt = malloc((SALT_LENGTH + 1) * sizeof(char));
  user->iter = ITERATION_COUNT;
  user->rights = 0;
  return user;
}

user_t* newUser(char* username, char* password, int rights) {
  user_t* user = initUser();


  // TODO: check for validity...
  user->username = username;
  user->rights = rights;

  if (RAND_bytes(user->salt, SALT_LENGTH) != 1) {
    printf("Random generator failed.\n");
    int err = ERR_get_error();
    char buf[256];
    ERR_error_string(err, buf);
    printf("Error: %s\n", buf);
  }

  user->salt[SALT_LENGTH] = '\0';


  (*user).passwordHash = createHash(password, user->salt, user->iter);
  return user;
}

uint8_t* createHash(char* password, uint8_t* salt, int iter) {
  unsigned char* out = malloc((SHA_512_DIGEST_SIZE + 1) * sizeof(char));
  memset(out,0,SHA_512_DIGEST_SIZE+1);
  if (PKCS5_PBKDF2_HMAC(password, strlen(password), /*(unsigned char*)*/ salt, SALT_LENGTH,
                            iter, EVP_sha512(), SHA_512_DIGEST_SIZE, out) != 1) {
    printf("Hashing pword failed.\n");
    return NULL;
  }
  else {
    return out;
  }
}

int checkCredentials(char* username, char* password) {
  user_t* user = getUserByName(username);
  if (user != NULL) {
    uint8_t* hash = createHash(password, user->salt, user->iter);
    for (int k = 0; k < SHA_512_DIGEST_SIZE; k++) {
      if (hash[k] != user->passwordHash[k]) return FALSE;
    }
    // This means the user has entered a correct password. In case the user
    // had an old iteration count, now is the time to change it.
    if (user->iter < ITERATION_COUNT) {
      printf("%s used a lower iter \n", user->username);
      user->passwordHash = createHash(password, user->salt, ITERATION_COUNT);
      user->iter = ITERATION_COUNT;
      printf("User iter is now %d\n", user->iter);
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

user_t* getUserByName(char* username) {
  // TODO: better data access
  for (int i = 0; i < db->count; i++) {
    if (!strcmp(db->table[i].username, username)) {
      return &db->table[i];
    }
  }
  return NULL;
}

int addUser(user_t* user) {
  db->count++;
  db->table[db->count-1] = *user;

  // pthread_mutex_lock(counterLock);
  // global_count++;
  // user->id = global_count;
  // pthread_mutex_unlock(counterLock);

  return 0;
}

char* bytesToHexString(uint8_t* bytes, int nbBytes) {
  char* hexString = malloc((2*nbBytes + 1)* sizeof(char));
  for (int i = 0; i < nbBytes; i++) {
    if (bytes[i] == '\0') break;
    sprintf(hexString+2*i, "%02x", bytes[i]);
  }
  hexString[2*nbBytes] = '\0';
  return hexString;
}

void printUser(user_t* user) {
  char* saltString = bytesToHexString(user->salt, SALT_LENGTH);
  char* hashString = bytesToHexString(user->passwordHash, SHA_512_DIGEST_SIZE);
  printf("----------------------------\n");
  printf("USER INFO\n");
  printf("Name: %s, \nHash: %s, \nid: %d, \nsalt: %s, \niter: %d, \nrights: %d\n", user->username, hashString, user->id, saltString, user->iter, user->rights);
  printf("----------------------------\n");

}

void updateIterationCount(int newCount) {
  ITERATION_COUNT = newCount;
}
