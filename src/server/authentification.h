#ifndef AUTHENTIFICATION
#define AUTHENTIFICATION

// BOOLEANS
#define FALSE 0
#define TRUE  1

// ERRORS
#define ERROR_USERNAME_TOO_LONG 1;

// TODO: think about overall structure. Maybe a struct holding all the user info?
// That way, you could define a max amount of users and its easy to give a new user
// an id -- jsut count the nbUsers and give the new one the next one.
//
// Maybe one also has to split this into authentification and a file accessing the
// user info database. Especially since some protection is needed.
// Moreover, check the user_t and import necessary libraries for SHA algorithm.
//
// Generally, the system has to be established. Currently, there are two user
// levels: normal and admin. Do they have access to all KVS or just their own?
// or a group of them? Depeding on that, determining access is just a boolean
// or a more complex function.
// Perhaps, it would be best to have a generic implementation with a member
// int rights, that determines the rights -- if we settle for just normal and
// admins, that's also possible.

/**
 * Structure with all necessary info for a user.
 * @member username the username string with max 30 chars
 * @member passwordHash the SHA-xxx hashvalue of their passwords
 * @member salt the corresponding salt
 * @member isAdmin boolean indicating whether this user has admin rights
 */
typedef struct {
  int id,                 // unique id given by the system
  char username[30],
  int passwordHash,
  int salt,
  int rights,             // 0 == adminRights, rest TBD
} user_t;

/**
 * Returns a pointer to a new empty user.
 * @return pointer to a user
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
 * @param  username of the desired user
 * @return          a pointer to the user info struct
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



#endif
