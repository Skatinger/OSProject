#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include "server.h"
#include "connection_handler.h"
#include "server_responses.h"
#include "../utils/logger.h"
#include "../utils/string_stuff.h"
#include "access_handler.h"


#if USE_TLS == TRUE
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#endif


// TODO: synchronization (make sure threads dont overwrite counter etc)
// TODO: find a useful data structure for saving theses threads (cf. connectionADT)
// TODO: sensibly handle a connection (read requests, treat them (perhaps via another
// implementation file), send a reply)
// TODO: implement an access control that determines when a connection may or may
// not access the KVS.
//
static int t_counter;
static pthread_mutex_t counter_lock;
static pthread_key_t USERNAME;  // this key can be used to have a thread-global value

int main(int argc, char const *argv[]) {
  char c;
  pthread_t pid;
  char root_pw[100];

  s_init_TLS();
  pthread_mutex_init(&counter_lock, NULL);

  get_password("Enter a password for the root account please\n", root_pw);
  printf("Thanks.\n");

  // new thread that waits for connections
  pthread_create(&pid, NULL, accept_new_connections, (void*)root_pw);

  // mechanism to stop the server
  c = getchar();
  if (c == 'c') {
    printf("Stopping.\n");
    end_access_handler();
    exit(0);
  }

  return 0;
}

void* accept_new_connections(void* arg) {
  t_counter = 0; // counting
  pthread_t tid;

  char* root_pw = (char*) arg;

  logger("Accepting connections now.", INFO);

  // this key can be used to give each thread a 'global' variable that is
  // specific to the thread and cannot be used by any other.
  pthread_key_create(&USERNAME, NULL);
  logger("Initialising access handler", INFO);

  // giving the acces handler this key, so it can tell what user is on the
  // current thread
  init_access_handler(&USERNAME, root_pw);

  // entering loop
  while(TRUE) {
    #if USE_TLS
      connection_t* con_info;
      pthread_mutex_lock(&counter_lock);
      /* critical region -- counter */
      if (t_counter < MAX_CONNECTIONS) {
        t_counter++;
        // counter manipulation over
        pthread_mutex_unlock(&counter_lock);

        // this blocks as long as there is no client trying to
        // connect to the server
        con_info = s_connect_TLS();
        if (con_info == NULL) continue;
        // thread to handle this connection
        logger("Creating thread for new connection", INFO);
        pthread_create(&tid, NULL, handle_TLS_connection, (void*)con_info);
      } else {
        // counter test done
        pthread_mutex_unlock(&counter_lock);
        logger("Can't accept any more connections atm", ERROR);
        sleep(RETRIAL_TIME);
      }
    #else
      logger("Trying to connect w/o TLS :0", ERROR);
      break;
    #endif
  }
}


void* handle_connection(void* arg) {
  // casting back to struct holding the data
  connection_t* con_info = (connection_t*) arg;
  time_t last_active;
  int r;

  last_active = time(NULL);

  // entering the main loop
  while(difftime(time(NULL), last_active < MAX_IDLE)) {
    // try to read from the network
    r = s_read(con_info);

    // now that the reading was successful, update the last active time
    last_active = time(NULL);

    if (r == 0) {
      logger(concat(2, "Reading from socket ", intToString(con_info->socket_descriptor)), INFO);
      //logger(con_info->buffer, INFO);
    } else {
      logger("Reading seems to have failed", ERROR);
      continue;
    }
  }
}

static char* parse_message(char* msg) {
  char* cmd = malloc(7*sizeof(char));

  // TODO: check for validity of the message (use regex to see if valid
  // message according to protocol)

  // retrieving the first word, i.e. the command of the request
  for (int i = 0; i < strlen(msg); i++) {
    if (msg[i] == ' ') {
      cmd[i] = 0;
      break;
    } else {
      cmd[i] = msg[i];
    }
  }

  // and switching over it
  if (!strcmp(cmd, "GET")) {
    return reader(getFirstParam(msg));
  } else if (!strcmp(cmd, "PUT")) {
    return writer(getFirstParam(msg), getSecondParam(msg), PUT);
  } else if (!strcmp(cmd, "DEL")) {
    return writer(getFirstParam(msg), NULL, DEL);
  } else if (!strcmp(cmd, "UPD")) {
    return writer(getFirstParam(msg), getSecondParam(msg), UPD);
  } else if (!strcmp(cmd, "ADD_U")) {
    return user_db_new(getFirstParam(msg), getSecondParam(msg));
  } else if (!strcmp(cmd, "DEL_U")) {
    return user_db_delete(getFirstParam(msg));
  } else if (!strcmp(cmd, "CHG_U")) {
    return user_db_update(getFirstParam(msg), getSecondParam(msg), getThirdParam(msg));
  } else if (!strcmp(cmd, "MK_ADM")) {
    return user_db_admin(getFirstParam(msg));
  } else if (!strcmp(cmd, "BYE")) {
    return BYE;
  } else if (!strcmp(cmd, "LOGIN")) {
    char* username = getFirstParam(msg);
    char* password = getSecondParam(msg);
    printf("Logging in user %s\n", username);
    pthread_setspecific(USERNAME, (void*) username);
    return login(password);
  } else if (!strcmp(cmd, "LOGOUT")) {
    return logout();
  } else {
    logger("Unknown command to parse:", INFO);
    logger(cmd, INFO);
    logger("from message:", INFO);
    logger(msg, INFO);
    return NULL;
  }
}

#if USE_TLS == TRUE
  void* handle_TLS_connection(void* arg) {
    // casting back to struct holding the data
    connection_t* con_info = (connection_t*) arg;
    time_t last_active;
    int r;
    char* msg;

    int failed_times = 0;
    last_active = time(NULL);

    // entering the main loop
    while(difftime(time(NULL), last_active) < MAX_IDLE) {
      // try to read from the network
      logger("trying to read from client", INFO);
      r = s_read_TLS(con_info);

      // now that the reading was successful, update the last active time
      last_active = time(NULL);

      if (r == 0) {
        logger(concat(2, "Reading from socket ", intToString(con_info->socket_descriptor)), INFO);
        //logger(con_info->buffer, INFO);
        logger("Replying the following:", INFO);

        char* msg = parse_message(con_info->buffer);
        if (msg == NULL) continue;

        logger(msg, INFO);

        if (!strcmp(msg, BYE)) {
          logger("Client ended the connection", INFO);
          logout();
          break;
        }

        s_write_TLS(con_info, msg);
      } else {
        logger("Reading seems to have failed", ERROR);
        failed_times++;
        if (failed_times > 5) {
          logger("Failed more than 5 times. Ending connection now.", ERROR);
          logout();
          break;
        }
        sleep(5);
        continue;
      }
    }

    // ending the conenction here
    s_end_TLS(con_info);
    pthread_mutex_lock(&counter_lock);
    t_counter--;
    pthread_mutex_unlock(&counter_lock);
    pthread_exit(NULL);

  }
#endif
