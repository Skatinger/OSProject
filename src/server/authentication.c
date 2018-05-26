#include <stdlib.h>
#include <stdio.h>
//#include <nettle/sha3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <pthread.h>
#include <string.h>
#include "authentication.h"




user_db_t* u_init_user_db() {
  user_db_t* db;
  db = malloc(sizeof(user_db_t));
  db->count = 0;
  db->store = kvs_create(MAXUSERS);
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
  user->current_thread = NULL;
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

int u_check_credentials(user_db_t* db, char* username, char* password) {
  user_t* user = get_user_by_name(db, username);
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

static user_t* get_user_by_name(user_db_t* db, char* username) {
  user_t* user;
  user = (user_t*) kvs_get(db->store, username);
  return user;
}

int u_add_user(user_db_t* db, char* username, char* password, int rights) {
  db->count++;
  user_t* user;
  user = new_user(db, password, rights);
  int rep;
  rep = kvs_set(db->store, username, (void*)user);

  if (rep == SUCCESS) {
    return 0;
  } else if (rep == NO_SUCH_KEY) {
    return ERROR_USERNAME_TAKEN;
  } else {
    return ERROR_GEN;
  }

  return 0;
}

int u_delete_user(user_db_t* db, char* username) {
  void* rep = kvs_del(db->store, username);
  int logged_in = get_user_by_name(db, username)->logged_in;

  if (rep != NULL && !logged_in) {
    db->count--;
    return SUCCESS;
  } else if (logged_in) {
    return ERROR_USER_LOGGEDIN;
  } else {
    return ERROR_GEN;
  }
}

int u_update_user(user_db_t* db, char* old_username, char* new_username, char* new_password) {
  int rights = get_user_by_name(db, old_username)->rights;
  int logged_in = get_user_by_name(db, old_username)->logged_in;

  if (logged_in) return ERROR_USER_LOGGEDIN;

  void* rep = kvs_del(db->store, old_username);
  if (rep == NULL) {
    return ERROR_GEN;
  }

  return u_add_user(db, new_username, new_password, rights);
}


int u_get_access(user_db_t* db, char* username) {
  user_t* user = get_user_by_name(db, username);

  pthread_t calling_thread = pthread_self();
  if (user == NULL || !user->logged_in || !pthread_equal(calling_thread, *user->current_thread)) {
    return NONE;
  } else {
    return user->rights;
  }
}

int u_get_logged_in(user_db_t* db, char* username) {
  user_t* user = get_user_by_name(db, username);

  pthread_t calling_thread = pthread_self();
  if (user == NULL) {
    return FALSE;
  } else {
    return user->logged_in;
  }
}

int u_set_access_rights(user_db_t* db, char* username, int rights) {
  user_t* user = get_user_by_name(db, username);
  if (user == NULL || rights < NONE || rights > ADMIN) {
    return 1;
  } else {
    user->rights = rights;
    return 0;
  }
}

int u_set_access_logged_in(user_db_t* db, char* username, int logged_in) {
  user_t* user = get_user_by_name(db, username);
  pthread_t* calling_thread = malloc(sizeof(pthread_t));
  *calling_thread = pthread_self();
  if (user == NULL || logged_in < FALSE || logged_in > TRUE) {
    return 1;
  } else {
    user->logged_in = logged_in;
    user->current_thread = logged_in ? calling_thread : NULL;
    return 0;
  }
}

char* u_bytes_to_hex_string(uint8_t* bytes, int nbBytes) {
  char* hexString = malloc((2*nbBytes + 1)* sizeof(char));
  for (int i = 0; i < nbBytes; i++) {
    if (bytes[i] == '\0') break;
    sprintf(hexString+2*i, "%02x", bytes[i]);
  }
  hexString[2*nbBytes] = '\0';
  return hexString;
}

void u_print_user(char* username, user_t* user) {
  char* saltString = u_bytes_to_hex_string(user->salt, SALT_LENGTH);
  char* hashString = u_bytes_to_hex_string(user->passwordHash, SHA_512_DIGEST_SIZE);
  printf("----------------------------\n");
  printf("USER INFO\n");
  printf("Name: %s, \nHash: %s, \nsalt: %s, \niter: %d, \nrights: %d\n", username, hashString, saltString, user->iter, user->rights);
  printf("----------------------------\n");
  free(saltString);
  free(hashString);
}

void u_print_user_jr(user_db_t* db, char* username) {
  user_t* user = get_user_by_name(db, username);
  if (user != NULL) u_print_user(username, user);
  else printf("User not found");
}

void u_update_iteration_count(user_db_t* db, int new_count) {
  db->ITERATION_COUNT = new_count;
}
