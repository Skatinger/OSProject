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
  printf("```````````````````````````````````````````````\n");
  printf("`Welcome to the trivial client implementation!`\n");
  printf("```````````````````````````````````````````````\n");
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
    printf("Getting did not work\n");
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
    printf("Hmm looks like that did not quite work\n");
  }
}

static void quit() {
  printf("Quitting\n");
  c_end_TLS();
  exit(0);
}

static void help() {
  printf("Usage: Type one of the following commands based on you want to do:\n");
  printf("\t * get\n \t * put \n \t * delete\n \t * quit\n\n");
}

static void put() {
  char key[1024];
  printf("Specify the key you want to add\n");
  scanf("%s", input);
  strcpy(key, input);
  printf("And the value for key %s\n", key);
  scanf("%s", input);
  c_send_TLS(PUT(key, input));
  c_receive_TLS(buffer);

  if (get_response_nr() == SUCCESS_PUT_NR) {
    printf("New key and value successfully set!\n");
  } else {
    printf("Seems like something has failed here...");
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
    printf("Nope, that wasn't it. Try again.\n");
    login();
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
