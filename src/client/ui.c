#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "client.h"
#include "client_requests.h"
#include "../server/server_responses.h"
#include "../utils/string_stuff.h"
#include "../utils/logger.h"

static char username[100];
static char password[100];
static char buffer[1024];
static char input[1024];
static char c;

int main(int argc, char const *argv[]) {
  init_logger("client", TRUE);
  // Beginning of the programm
  if (argc < 2) {printf("needs an IP address!\n"); return 1;}
  printf("\n\n");
  printf(GREEN_TXT(BOLD ASCII_ART));
  printf("\n\n");
  printf(GREEN_TXT(BOLD "```````````````````````````````````````````````\n"));
  printf(GREEN_TXT(BOLD "`Welcome to the trivial client implementation!`\n"));
  printf(GREEN_TXT(BOLD "```````````````````````````````````````````````\n"));
  printf("\n");
  printf(GREEN_TXT("Like this Greek mythological messenger, it will carry\n"));
  printf(GREEN_TXT("what you have to say to the server!\n\n"));



  // connecting to the server
  printf(">> Trying to connect to %s\n", argv[1]);
  logger("IP-address of server:", INFO);
  logger(argv[1], INFO);
  c_connect_TLS(argv[1]);

  // and login
  logger("Asking user to login", INFO);
  login();

  // now entering "main loop", where the cmdline waits for user input
  // and processes it with the corresponding functions.
  printf("Enter a command. 'help' will print help.\n");
  while (TRUE) {
    printf("\n$ ");
    scanf("%s", input);
    logger(concat(2, "UINPUT: ", input), INFO);

    // CASE DISTINCTION:
    if (!strcmp(input, "help")) {
      help();
      continue;
    }

    else if (!strcmp(input, "quit")) {
      quit();
      continue;
    }

    else if (!strcmp(input, "get")) {
      get();
      continue;
    }

    else if (!strcmp(input, "put")) {
      put();
      //logger("after put()", INFO);
      continue;
    }

    else if (!strcmp(input, "update")) {
	  update();
	  continue;
	}

    else if (!strcmp(input, "addUser")) {
      add_user();
      //logger("after add_user", INFO);
      continue;
    }

    else if(!strcmp(input, "deleteUser")) {
      delete_user();
      continue;
    }

    else if (!strcmp(input, "updateUser")) {
      update_user();
      continue;
    }

    else if(!strcmp(input, "makeAdmin")) {
      make_admin();
      continue;
    }
    else if (!strcmp(input, "delete")) {
      delete();
      continue;
    }

    else if (!strcmp(input, "logout")) {
      logout();
      continue;
    }

  }
  return 0;
}

static void get() {
  printf("Specify the key you want to get\n");
  scanf("%s", input);
  logger(concat(2, "UINTPUT: ", input), INFO);
  c_send_TLS(GET(input));
  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_GOT_NR) {
    printf("The corresponding value is: \n");
    printf("\t%s\n", getSecondParam(buffer));
  } else {
    print_error_message();
  }
}

static void delete() {
  printf("Specify the key you want to delete\n");
  scanf("%s", input);
  logger(concat(2, "UINTPUT: ", input), INFO);
  c_send_TLS(DEL(input));
  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_DEL_NR) {
    printf("This key has vanished into thin air, as desired\n");
  } else {
    print_error_message();
  }
}

static void update() {
  char key[1024];
  printf("Specify the key you want to update\n");
  scanf("%s", input);
  logger(concat(2, "UINTPUT: ", input), INFO);
  strcpy(key, input);
  printf("And the value for key %s\n", key);
  scanf("%s", input);
  logger(concat(2, "UINTPUT: ", input), INFO);
  c_send_TLS(UPD(key, input));
  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_UPD_NR) {
    printf("Value to key successfully updated!\n");
  } else {
    print_error_message();
  }
}

static void quit() {
  // TODO: implement logging out first (in server)
  printf("Logging out\n");
  logger("logging out & quitting", INFO);
  c_send_TLS(LOGOUT(username));
  printf("Quitting\n");
  c_end_TLS();
  close_logger();
  exit(0);
}

static void help() {
  printf("Usage: Type one of the following commands based on you want to do:\n");
  printf("\t * get\n \t * put \n \t * update \n \t * delete\n \t * logout\n \t * quit\n\n");
  printf("If you have admin rights, you can also do the following:\n");
  printf("\t * addUser\n \t * makeAdmin\n \t * updateUser\n\n");
}

static void put() {
  char key[1024];
  printf("Specify the key you want to add\n");
  scanf("%s", input);
  logger(concat(2, "UINTPUT: ", input), INFO);
  strcpy(key, input);
  printf("key %s accepted\n", input);
  printf("And the value for key %s\n", key);
  scanf("%s", input);
  logger(concat(2, "UINTPUT: ", input), INFO);
  c_send_TLS(PUT(key, input));
  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_PUT_NR) {
    printf("New key and value successfully set!\n");
  } else {
    print_error_message();
    logger("put(): printed error", INFO);
  }
}

static void login() {
  printf("Please provide your username:\n");
  scanf("%s", username);
  char c = getchar();
  get_password("Please provide your password\n", password);
  printf("Thanks. Checking ...\n");

  c_send_TLS(LOGIN(username, password));
  memset(password, 0, 100);
  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);
  if (!strcmp(buffer, SUCCESS_LOGIN(username))) {
    printf("Succesfully logged in!\n");
    return;
  } else if (!strcmp(buffer, ERROR_ALREADY_LOGGEDIN(username))) {
    print_error_message();
    //login();
  } else {
    printf(RED_TXT("Nope, that wasn't it. Try again.\n"));
    //login();
  }
}

static void add_user() {
  char username[1024];
  char password[1024];
  char c;
  //char* access_rights
  //todo validate root password again before allowing changes to user database
  printf("Name for new user:\n");
  scanf("%s", input);
  strcpy(username, input);
  c = getchar();
  get_password("password new user:\n", password);
  c_send_TLS(ADD_U(username,password));

  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_ADD_U_NR) {
    printf("Congrats, you added User %s\n", username);
  } else {
    print_error_message();
    //logger("add_user(): after error", INFO);
  }
}

static void delete_user() {
  char username[1024];
  char c;
  //char* access_rights
  //todo validate root password again before allowing changes to user database
  printf("Please enter the username of the user you want to delete:\n");
  scanf("%s", input);
  strcpy(username, input);

  c_send_TLS(DEL_U(username));
  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_DEL_U_NR) {
    printf("Congrats, you deleted User %s\n", username);
  } else {
    print_error_message();
    //logger("delete_user(): after error", INFO);
  }
}

static void make_admin() {
  char username[1024];
  char c;
  //char* access_rights
  //todo validate root password again before allowing changes to user database
  printf("Please enter the username of the user you want make an admin:\n");
  scanf("%s", input);
  strcpy(username, input);

  c_send_TLS(MK_ADMIN(username));
  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_MK_ADM_NR) {
    printf("Congrats, you made User %s admin\n", username);
  } else {
    print_error_message();
    //logger("mkadm(): after error", INFO);
  }
}

static void update_user() {
  char old_username[1024];
  char new_username[1024];
  char new_password[1024];
  char c;
  //char* access_rights
  //todo validate root password again before allowing changes to user database
  printf("Which user do you want to update? Enter their username, please:\n");
  scanf("%s", input);
  strcpy(old_username, input);
  printf("What should their new username be?\n");
  scanf("%s", input);
  strcpy(new_username, input);
  c = getchar();
  get_password("Enter the new password:\n", password);
  c_send_TLS(CHG_U(old_username, new_username, password));

  c_receive_TLS(buffer);
  logger(concat(2, "server replied ", buffer), INFO);

  if (get_response_nr() == SUCCESS_CHG_U_NR) {
    printf("Congrats, you updated User %s to %s\n", old_username, new_username);
  } else {
    print_error_message();
    //logger("upd_user(): after error", INFO);
  }
}


static void logout() {
  printf("Logging you out\n");
  logger("User logging out\n", INFO);
  c_send_TLS(LOGOUT(username));
  c_receive_TLS(buffer);
  printf("You may now login with a different account\n");
  login();
}

static int get_response_nr() {
  char response[4];
  for (int i = 0; i < strlen(buffer); i++) {
    if (buffer[i] == ' ') {
      response[i] = 0;
      break;
    } else {
      response[i] = buffer[i];
    }
  }
  return atoi(response);
}

static void print_error_message() {
  int n = get_response_nr();
  switch (n) {
    case ERROR_KEY_NOT_FOUND_NR:
      printf(RED_TXT("There's no such key in sight!\n"));
      printf("Hint: Add it or look for another one.\n");
      break;
    case ERROR_KEY_INVALID_NR:
      printf(RED_TXT("This key is not valid.\n"));
      printf("Hint: it probably contained : or ; or some other weird stuff.\n");
      break;
    case ERROR_VALUE_INVALID_NR:
      printf(RED_TXT("This value is not valid.\n"));
      printf("Hint: it probably contained : or ; or some other weird stuff.\n");
      break;
    case ERROR_KEY_OCCUPIED_NR:
      printf(RED_TXT("Too late! This key is already taken!\n"));
      break;
    case ERROR_ACCESS_DENIED_NR:
      printf(RED_TXT("DEEE-NIED!\n"));
      printf(RED_TXT(BOLD "\n                           ,---.\n                          /    |\n                         /     |\n     Gandalf            /      |\n                       /       |\n                  ___,'        |\n                <  -'          :\n                 `-.__..--'``-,_\\_\n                    |o/ <o>` :,.)_`>\n                    :/ `     ||/)\n                    (_.).__,-` |\\\n                    /( `.``   `| :\n                    \\'`-.)  `  ; ;\n                    | `       /-<\n                    |     `  /   `.\n    ,-_-..____     /|  `    :__..-'\\\n   /,'-.__\\\\  ``-./ :`      ;       \\\n   `\\ `\\  `\\\\  \\ :  (   `  /  ,   `. \\\n     \\` \\   \\\\   |  | `   :  :     .\\ \\\n      \\ `\\_  ))  :  ;     |  |      ): :\n     (`-.-'\\ ||  |\\ \\   ` ;  ;       | |\n      \\-_   `;;._   ( `  /  /_       | |\n       `-.-.// ,'`-._\\__/_,'         ; |\n          \\:: :     /     `     ,   /  |\n           || |    (        ,' /   /   |\n           ||                ,'   / SSt|\n\n"));
      printf(RED_TXT("YOU SHALL NOT PASS! You don't have access to this.\n"));
      printf("Hint: Probably not logged in or something.\n");
      break;
    case ERROR_LOGOUT_FAIL_NR:
      printf(RED_TXT("You have not been logged out. Something failed\n"));
      break;
    case ERROR_USER_OCCUPIED_NR:
      printf(RED_TXT("This username taken\n"));
      break;
    case ERROR_USERNAME_INVALID_NR:
      printf(RED_TXT("This username is valid!\n"));
      break;
    case ERROR_NO_ADMIN_NR:
      printf(RED_TXT("Sorry honey, you ain't no admin!\n"));
      printf("Hint: Buy the system admin chocolate and or vodka, maybe they will give you the rights.\n");
      break;
    case ERROR_SERVER_FULL_NR:
      printf(RED_TXT("Oh boy, that server's full girl!\n"));
      printf("Call the dev. This is bad.\n");
      break;
    case ERROR_USER_MODIFICATION_NR:
      printf(RED_TXT("The modification you tried to make somehow failed. Perhaps this user doesn't exist.\n"));
      break;
    case ERROR_USER_NOT_UPDATABLE_NR:
      printf(RED_TXT("Nope, can't update while this user is logged in.\n"));
      break;
    case ERROR_ALREADY_LOGGEDIN_NR:
      printf(RED_TXT("Thise usere ise alredy loggede ine, scusi...\n"));
      printf("Hint: log out on the other machine first.\n");
      break;
    default:
      printf(RED_TXT("Congrats! You managed to make an error that the devs did not think of!\n"));
      printf("Hint: no clue...\n");
      break;
  }
  //logger("Printed error", INFO);
}
