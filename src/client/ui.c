#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "client.h"
#include "clientRequests.h"
#include "../server/serverResponses.h"
#include "../utils/string_stuff.h"

static char username[100];
static char password[100];
static char buffer[1024];
static char input[1024];
static char c;

int main(int argc, char const *argv[]) {
  // Beginning of the programm
  if (argc < 2) {printf("needs an IP address!\n"); return 1;}
  printf("\n\n");
  printf(GREEN_TXT(BOLD "```````````````````````````````````````````````\n"));
  printf(GREEN_TXT(BOLD "`Welcome to the trivial client implementation!`\n"));
  printf(GREEN_TXT(BOLD "```````````````````````````````````````````````\n"));
  printf("\n\n");

  // connecting to the server
  printf(">> Trying to connect to %s\n", argv[1]);
  c_connect_TLS(argv[1]);

  // and login
  login();

  // now entering "main loop", where the cmdline waits for user input
  // and processes it with the corresponding functions.
  printf("Enter a command. 'help' will print help.\n");
  while (TRUE) {
    printf("$ ");
    scanf("%s", input);

    // CASE DISTINCTION:
    if (!strcmp(input, "help")) {
      help();
      continue;
    }

    else if (!strcmp(input, "quit")) {
      quit();
    }

    else if (!strcmp(input, "get")) {
      get();
      continue;
    }

    else if (!strcmp(input, "put")) {
      put();
      continue;
    }

    else if (!strcmp(input, "update")) {
	  update();
	  continue;
	}

    else if (!strcmp(input, "addUser")) {
      add_user();
    }

    else if (!strcmp(input, "delete")) {
      delete();
      continue;
    }

  }
  return 0;
}

static void get() {
  printf("Specify the key you want to get\n");
  scanf("%s", input);
  c_send_TLS(GET(input));
  c_receive_TLS(buffer);

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
  c_send_TLS(DEL(input));
  c_receive_TLS(buffer);

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
  strcpy(key, input);
  printf("And the value for key %s\n", key);
  scanf("%s", input);
  c_send_TLS(UPD(key, input));
  c_receive_TLS(buffer);

  if (get_response_nr() == SUCCESS_UPD_NR) {
    printf("Value to key successfully updated!\n");
  } else {
    print_error_message();
  }
}

static void quit() {
  // TODO: implement logging out first (in server)
  //printf("Logging out\n");
  //c_send_TLS(LOGOUT(username));
  printf("Quitting\n");
  c_end_TLS();
  exit(0);
}

static void help() {
  printf("Usage: Type one of the following commands based on you want to do:\n");
  printf("\t * get\n \t * put \n \t * update \n \t * delete\n \t * addUser\n \t * quit\n\n");
}

static void put() {
  char key[1024];
  printf("Specify the key you want to add\n");
  scanf("%s", input);
  strcpy(key, input);
  printf("key %s accepted\n", input);
  printf("And the value for key %s\n", key);
  scanf("%s", input);
  c_send_TLS(PUT(key, input));
  c_receive_TLS(buffer);

  if (get_response_nr() == SUCCESS_PUT_NR) {
    printf("New key and value successfully set!\n");
  } else {
    print_error_message();
  }
}

static void login() {
  printf("Please provide your username:\n");
  scanf("%s", username);
  char c = getchar();
  get_password("Please provide your password\n", password);
  printf("Thanks. Checking ...\n");

  c_send_TLS(LOGIN(username, password));
  c_receive_TLS(buffer);
  if (!strcmp(buffer, SUCCESS_LOGIN(username))) {
    printf("Succesfully logged in!\n");
    return;
  } else {
    printf(RED_TXT("Nope, that wasn't it. Try again.\n"));
    login();
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

  if (get_response_nr() == SUCCESS_ADD_U_NR) {
    printf("Congrats, you added User %s\n", username);
  } else {
    print_error_message();
  }
}

static int get_response_nr() {
  char response[3];
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
      printf(RED_TXT("There's no such key in site!\n"));
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
      printf(RED_TXT("YOU SHALL NOT PASS! DEE-NIED! You don't have access to this.\n"));
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
      printf(RED_TXT("Sorry honey, you aint no admin!\n"));
      printf("Hint: Buy the system admin chocolate and or vodka, maybe they will give you the rights.\n");
      break;
    case ERROR_SERVER_FULL_NR:
      printf(RED_TXT("Oh boy, that server's full girl!\n"));
      printf("Call the dev. This is bad.\n");
      break;
    default:
      printf(RED_TXT("Congrats! You managed to make an error that the devs did not think of!\n"));
      printf("Hint: no clue...\n");
      break;
  }
}
