#include "../server/authentification.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>



int main(int argc, char const *argv[]) {
  initUserHandler();

 user_db_t* db = initUserDB();
 user_db_t* seconDB = initUserDB();


 printf("Adding a few  users to both dbs.\n");
 user_t* me = newUser("Remo", "SehrKomplexes_pw67#*", 0);
 user_t* secondUser = newUser("eineAnderPerson", "1234", 3);
 user_t* third = newUser("Roger Federer", "!08jdhsl@*)", 2);
 user_t* last = newUser("user17", "charliecharli", 0);

 addUser(db, me);
 addUser(seconDB, me);
 addUser(db, third);
 addUser(db, last);

 printf("Printing all the user info\n");
 printUser(me);
 printUser(secondUser);
 printUser(third);
 printUser(last);

 printf("Printing all the users in the first db\n");
 for (int i = 0; i < db->count; i++) {
   printUser(&db->table[i]);
 }
 printf("and of the second\n");
 for (int i = 0; i < seconDB->count; i++) {
   printUser(&seconDB->table[i]);
 }

 printf("Checking credentials\n");
 printf("Checking username \"chucknorris\" with password \"abcd\" in db1\n");
 printf("%d\n", checkCredentials(db, "chucknorris", "abcd"));

 printf("Checking username \"Remo\" with password \"LoLoLoL\" in db1\n");
 printf("%d\n", checkCredentials(db, "Remo", "LoLoLoL"));

 printf("Checking username \"Remo\" with password \"SehrKomplexes_pw67#*\" in db1\n");
 printf("%d\n", checkCredentials(db, "Remo", "SehrKomplexes_pw67#*"));


 exit(0);
  return 0;
}
