#include <stdlib.h>
#include <stdio.h>
//#include <nettle/sha3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <pthread.h>
#include <string.h>
#include "authentification.h"




user_db_t* init_user_db() {
  user_db_t* db;
  db = malloc(sizeof(user_db_t));
  db->count = 0;
  db->store = create(MAXUSERS);
  // apparently this is a good value (google)
  db->ITERATION_COUNT = 100000;
  return db;
}

static user_t* init_user(user_db_t* db) {
  user_t* user = malloc(sizeof(user_t));
  user->passwordHash = malloc(SHA_512_DIGEST_SIZE * sizeof(uint8_t));
  user->salt = malloc((SALT_LENGTH + 1) * sizeof(char));
  user->iter = db->ITERATION_COUNT;
  user->rights = NONE;
  user->logged_in = FALSE;
  return user;
}

static user_t* new_user(user_db_t* db, char* password, int rights) {
  user_t* user = init_user(db);

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

static uint8_t* createHash(char* password, uint8_t* salt, int iter) {
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

int checkCredentials(user_db_t* db, char* username, char* password) {
  user_t* user = getUserByName(db, username);
  if (user != NULL) {
    uint8_t* hash = createHash(password, user->salt, user->iter);
    for (int k = 0; k < SHA_512_DIGEST_SIZE; k++) {
      if (hash[k] != user->passwordHash[k]) return FALSE;
    }
    // This means the user has entered a correct password. In case the user
    // had an old iteration count, now is the time to change it.
    if (user->iter < db->ITERATION_COUNT) {
      printf("%s used a lower iter \n", username);
      user->passwordHash = createHash(password, user->salt, db->ITERATION_COUNT);
      user->iter = db->ITERATION_COUNT;
      printf("User iter is now %d\n", user->iter);
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

static user_t* getUserByName(user_db_t* db, char* username) {
  user_t* user;
  user = (user_t*) get(db->store, username);
  return user;
}

int addUser(user_db_t* db, char* username, char* password, int rights) {
  db->count++;
  user_t* user;
  user = new_user(db, password, rights);
  int rep;
  rep = set(db->store, username, (void*)user);

  if (rep == SUCCESS) {
    return 0;
  } else if (rep == KEY_IN_USE_ERROR) {
    return ERROR_USERNAME_TAKEN;
  } else {
    return ERROR_GEN;
  }

  return 0;
}

int delete_user(user_db_t* db, char* username) {
  void* rep = del(db->store, username);
  if (rep != NULL) {
    db->count--;
    return 0;
  } else {
    return 1;
  }
}

int update_user(user_db_t* db, char* old_username, char* new_username, char* new_password) {
  int rights = getUserByName(db, old_username)->rights;
  int logged_in = getUserByName(db, old_username)->logged_in;

  if (logged_in) return ERROR_USER_LOGGEDIN;

  void* rep = del(db->store, old_username);
  if (rep == NULL) {
    return ERROR_GEN;
  }

  return addUser(db, new_username, new_password, rights);
}


int get_access(user_db_t* db, char* username) {
  user_t* user = getUserByName(db, username);
  if (user == NULL || !user->logged_in) {
    return NONE;
  } else {
    return user->rights;
  }
}

int set_access_rights(user_db_t* db, char* username, int rights) {
  user_t* user = getUserByName(db, username);
  if (user == NULL || rights < NONE || rights > ADMIN) {
    return 1;
  } else {
    user->rights = rights;
    return 0;
  }
}

int set_access_logged_in(user_db_t* db, char* username, int logged_in) {
  user_t* user = getUserByName(db, username);
  if (user == NULL || logged_in < FALSE || logged_in > TRUE) {
    return 1;
  } else {
    user->logged_in = logged_in;
    return 0;
  }
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

void printUser(char* username, user_t* user) {
  char* saltString = bytesToHexString(user->salt, SALT_LENGTH);
  char* hashString = bytesToHexString(user->passwordHash, SHA_512_DIGEST_SIZE);
  printf("----------------------------\n");
  printf("USER INFO\n");
  printf("Name: %s, \nHash: %s, \nsalt: %s, \niter: %d, \nrights: %d\n", username, hashString, saltString, user->iter, user->rights);
  printf("----------------------------\n");
  free(saltString);
  free(hashString);
}

void print_user(user_db_t* db, char* username) {
  user_t* user = getUserByName(db, username);
  if (user != NULL) printUser(username, user);
  else printf("User not found");
}

void updateIterationCount(user_db_t* db, int newCount) {
  db->ITERATION_COUNT = newCount;
}
