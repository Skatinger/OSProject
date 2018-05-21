#ifndef AUTHENTIFICATION
#define AUTHENTIFICATION

#include "../project.h"
#include "key_value.h"
#include <stdint.h>
#include <pthread.h>

// ERRORS
#define ERROR_USERNAME_TOO_LONG 1
#define ERROR_USERNAME_TAKEN 2
#define ERROR_GEN 3

#define MAXUSERS 1000
#define SALT_LENGTH 64
#define ALPHABET_SIZE 26

#define ADMIN 1
#define NORMAL 0
#define NONE -1



#define SHA_512_DIGEST_SIZE 64

/**
 * Structure with all necessary info for a user.
 * @member passwordHash the PBKDF2 generated hash of the user's pw
 * @member salt the corresponding salt
 * @memeber iter the number of iterations for this user in the pw hashing algo
 * @member rights indicating whether this user has admin rights
 * @member logged_in whether this user is currently logged in
 */
typedef struct {
  uint8_t* passwordHash;
  uint8_t* salt;
  int iter;
  int rights;             // see macros
  int logged_in;
} user_t;

/**
 * Wrapper structure for the user database.
 * @member count the number of users
 * @member store the keyvaluestore used to store users.
 * @member ITERATION_COUNT the usual iteration count used for hashing pws.
 */
typedef struct {
  int count;
  KVS* store;
  int ITERATION_COUNT;
} user_db_t;

/**
 * Initialises the user database.
 * @return pointer to the new "db".
 */
user_db_t* initUserDB();
/**
 * Returns a pointer to a new empty user.
 * @param db the db where the user will reside
 * @return pointer to an empty user
 */
static user_t* initUser(user_db_t* db);

/**
 * Creates a new user based on the given arguments
 * @param db the db where the user will be saved
 * @param  password in String form, hash will be calculated
 * @param  rights  integer info about the rights a user has (admin, nah, etc.)
 * @return          a pointer to the newly created user
 */
static user_t* newUser(user_db_t* db, char* password, int rights);

/**
 * Adds a new user to the database based on the given arguments.
 * @param  db the database to add the user to
 * @param username the username of the new user
 * @param password the pw of the new user
 * @param rights the rights this user gets
 * @return    0 if successful, 1 if name too long, 2 if name taken, 3 for other
 *              errors.
 */
int addUser(user_db_t* db, char* username, char* password, int rights);

/**
 * Get user based on their username.
 * @param the db to search in
 * @param  username of the desired user
 * @return a pointer to the user info struct
 */
static user_t* getUserByName(user_db_t* db, char* username);

/**
 * Checks username and password with the database to determine acces rights.
 * Hashes the password to compare with the stored user_t.
 * @param db the database to check in
 * @param  username the string with the username to test
 * @param  password string with the password to be checked
 * @return          FALSE if credentials are flase, TRUE if they are correct.
 */
int checkCredentials(user_db_t* db, char* username, char* password);

/**
 * Determines if the user has access // what kind of access
 * @param db the database to check
 * @param  username the username to check
 * @return      int specifing the type of access granted
 *              ADMIN:  admin rights
 *              NORMAL: rights of a normal user
 *              NONE:   the user isn't logged in.
 */
int get_access(user_db_t* db, char* username);

/**
 * Sets the rights member of a user (i.e. if their rights are updated).
 * @param  username the user to update
 * @param  rights   their new rights
 * @return          0 success, 1 otherwise
 */
int set_acces_rights(user_db_t* db, char* username, int rights);

/**
 * Sets the logged_in member of a user (i.e. if they are logged in).
 * @param  username the user to update
 * @param  logged_in   TRUE iff they're logged in
 * @return          0 success, 1 otherwise
 */
int set_access_logged_in(user_db_t db, char* username, int logged_in)

/**
 * Prints all the necessary user information to stdout.
 * @param user the user to print
 */
void printUser(user_t* user);

/**
 * Auxlilary function that takes an array of bytes and turns it into a human-
 * readable hex-string.
 * @param  bytes   an array of bytes
 * @param  nbBytes the number of Bytes to be turned into a string
 * @return    the string.
 */
char* bytesToHexString(uint8_t* bytes, int nbBytes);


/**
 * Creates a hash from the given password and salt. Uses the widely used legacy
 * pbkdf2 algorithm with iter iterations to make the hashing slower, thus
 * cracking a password more difficult.
 *
 * the number of iterations may vary from user to user (this is so that the number can
 * be increased with better hardware in the future), that's why it is passed as
 * an argument.
 * @param  password A string with the password to hash
 * @param  salt     An array of random bytes unique to each user
 * @param  iter     the number of iterations to do
 * @return          the array with the byte hash
 */
static uint8_t* createHash(char* password, uint8_t* salt, int iter);

/**
 * Makes it possible to update the iteration count for the hash algorithm.
 * @param db the concerned user db
 * @param newCount the new iteration count.
 */
void updateIterationCount(user_db_t* db, int newCount);
#endif
