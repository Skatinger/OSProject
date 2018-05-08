#ifndef AUTHENTIFICATION
#define AUTHENTIFICATION

#include "../project.h"

// ERRORS
#define ERROR_USERNAME_TOO_LONG 1

#define MAXUSERS 1000
#define SALT_LENGTH 64
#define ALPHABET_SIZE 26

#define ADMIN 0



#define SHA_512_DIGEST_SIZE 64

#include <stdint.h>
#include <pthread.h>

// TODO: think about overall structure. Maybe a struct holding all the user info?
// That way, you could define a max amount of users and its easy to give a new user
// an id -- just count the nbUsers and give the new one the next one.
//
// Maybe one also has to split this into authentication and a file accessing the
// user info database. Especially since some protection is needed.
// Moreover, check the user_t and import necessary libraries for SHA algorithm.
//
// Generally, the system has to be established. Currently, there are two user
// levels: normal and admin. Do they have access to all KVS or just their own?
// or a group of them? Depending on that, determining access is just a boolean
// or a more complex function.
// Perhaps, it would be best to have a generic implementation with a member
// int rights, that determines the rights -- if we settle for just normal and
// admins, that's also possible.

// TODO: better interface for db -- use binary tree or hashTable or sth.

/**
 * Structure with all necessary info for a user.
 * @member username the username string with max 30 chars
 * @member passwordHash the PBKDF2 generated hash of the user's pw
 * @member salt the corresponding salt
 * @member iter the number of iterations for this user in the pw hashing algorithm
 * @member rights indicating whether this user has admin rights
 */
typedef struct {
  int id;                 // unique id given by the system
  char* username;
  uint8_t* passwordHash;
  uint8_t* salt;
  int iter;
  int rights;             // 0 == adminRights, rest TBD
} user_t;


typedef struct {
  int count;
  user_t* table;
} user_db_t;

/**
 * Integer that counts the total number of all users, used for giving ids.
 * @return [description]
 */
extern int global_count;
extern pthread_mutex_t* counterLock;

void initUserHandler();

/**
 * Initialises the user database.
 * @return pointer to the new "db".
 */
user_db_t* initUserDB();
/**
 * Returns a pointer to a new empty user.
 * @return pointer to an empty user
 */
user_t* initUser();

/**
 * Creates a new user based on the given arguments
 * @param  username String (max 30 chars) for the username
 * @param  password in String form, hash will be calculated
 * @param  rights  integer info about the rights a user has (admin, nah, etc.)
 * @return          a pointer to the newly created user
 */
user_t* newUser(char* username, char* password, int rights);

/**
 * Adds a new user to the database.
 * @param  user pointer to a user struct to be added
 * @return    0 if successful,
 *            1 if error
 */
int addUser(user_t* user);

/**
 * Find a user based on their unique id in the database.
 * @param  id the unique id
 * @return    a pointer to the user info struct
 */
user_t* getUserById(int id);

/**
 * Get user based on their username.
 * @param the db to search in
 * @param  username of the desired user
 * @return a pointer to the user info struct
 */
user_t* getUserByName(char* username);

/**
 * Checks username and password with the database to determine acces rights.
 * Hashes the password to compare with the stored user_t.
 * @param  username the string with the username to test
 * @param  password string with the password to be checked
 * @return          FALSE if credentials are flase, TRUE if they are correct.
 */
int checkCredentials(char* username, char* password);

/**
 * Determines if the user has access // what kind of access
 * @param  user pointer to a user type
 * @return      int specifing the type of access granted
 */
int hasAccess(user_t* user);

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
uint8_t* createHash(char* password, uint8_t* salt, int iter);

void updateIterationCount(int newCount);
#endif
