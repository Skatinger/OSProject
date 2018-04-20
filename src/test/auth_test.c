#include "../server/authentification.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>



int main(int argc, char const *argv[]) {
  initUserHandler();

  user_db_t* db = initUserDB();


 printf("Adding a few  users to both dbs.\n");
 user_t* me = newUser("Remo", "SehrKomplexes_pw67#*", 0);
 user_t* secondUser = newUser("eineAnderPerson", "1234", 3);
 user_t* secondsecondUser = newUser("anderPerson,gleichesPasswort", "1234", 3);
 user_t* third = newUser("Roger Federer", "!08jdhsl@*)", 2);
 user_t* last = newUser("user17", "charliecharli", 0);

 addUser(me);
 addUser(third);
 addUser(last);
 addUser(secondUser);
 addUser(secondsecondUser);


 //printf("Printing all the user info\n");
 // printUser(me);
 // printUser(secondUser);
 // printUser(third);
 // printUser(last);

 printf("Printing all the users in the first db\n");
 for (int i = 0; i < db->count; i++) {
   printUser(&db->table[i]);
 }


 printf("Checking credentials\n");
 printf("Checking username \"chucknorris\" with password \"abcd\" in db1\n");
 printf("%d\n", checkCredentials("chucknorris", "abcd"));

 printf("Checking username \"Remo\" with password \"LoLoLoL\" in db1\n");
 printf("%d\n", checkCredentials("Remo", "LoLoLoL"));

 printf("Checking username \"Remo\" with password \"SehrKomplexes_pw67#*\" in db1\n");
 printf("%d\n", checkCredentials("Remo", "SehrKomplexes_pw67#*"));

 printf("\nNow changing the number of iterations to 200 000\n");
 updateIterationCount(200000);

 printf("Logging in username \"Remo\", which updates his iteration info\n");
 printf("%d\n", checkCredentials("Remo", "SehrKomplexes_pw67#*"));
 printf("See if that worked\n");
 printUser(&db->table[0]);

 printf("Now trying to log in again with new iter\n");
 printf("%d\n", checkCredentials("Remo", "SehrKomplexes_pw67#*"));

printf("Trying to get user by name (\"Remo\")\n");
user_t* remo = getUserByName("Remo");

printUser(remo);

 exit(0);
  return 0;
}
