#include "../server/authentification.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>



int main(int argc, char const *argv[]) {

  user_db_t* db = init_user_db();


 printf("Adding a few  users to both dbs.\n");
 addUser(db, "Remo", "SehrKomplexes_pw67#*", 0);
 addUser(db, "eineAnderPerson", "1234", 3);
 addUser(db, "anderPerson,gleichesPasswort", "1234", 3);
 addUser(db, "Roger Federer", "!08jdhsl@*)", 2);
 addUser(db, "user17", "charliecharli", 0);


 printf("Checking credentials\n");
 printf("Checking username \"chucknorris\" with password \"abcd\" in db1\n");
 printf("%d\n", checkCredentials(db, "chucknorris", "abcd"));

 printf("Checking username \"Remo\" with password \"LoLoLoL\" in db1\n");
 printf("%d\n", checkCredentials(db, "Remo", "LoLoLoL"));

 printf("Checking username \"Remo\" with password \"SehrKomplexes_pw67#*\" in db1\n");
 printf("%d\n", checkCredentials(db, "Remo", "SehrKomplexes_pw67#*"));

 printf("\nNow changing the number of iterations to 200 000\n");
 updateIterationCount(db, 200000);

 printf("Logging in username \"Remo\", which updates his iteration info\n");
 printf("%d\n", checkCredentials(db, "Remo", "SehrKomplexes_pw67#*"));
 printf("See if that worked\n");
 print_user(db, "Remo");

 printf("Now trying to log in again with new iter\n");
 printf("%d\n", checkCredentials(db, "Remo", "SehrKomplexes_pw67#*"));


 exit(0);
  return 0;
}
