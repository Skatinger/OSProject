#include <unistd.h>
#include "../client/client.h"
#include "../client/clientRequests.h"
#include "../utils/logger.h"
#include "../utils/string_stuff.h"


int main(int argc, char *argv[]) {
  if(argc != 2) {
      printf("\n Usage: %s <ip of server> \n",argv[0]);
      return 1;
  }

  c_connect_TLS(argv[1]);

  printf("Enter the username please\n");
  char username[100];
  char pw[100];
  scanf("%s", username);
  char c = getchar(); // newline char
  get_password("Enter the password please\n", pw);

  char* message = LOGIN(username, pw);

  printf("Sending %s\n", message);
  c_send_TLS(message);
  char buf[BUFFER_SIZE];
  int n = c_receive_TLS(buf);

  if(fputs(buf, stdout) == EOF) {
      printf("\n Error : Fputs error\n");
  }

  // write a simple request
  message = PUT("unifr", "OPisgreat");

  sleep(2); // wait until message is sent to give some time to the testing
             // programmer
             //
  printf("Sending %s\n", message);
  c_send_TLS(message);
  n = c_receive_TLS(buf);

  if(fputs(buf, stdout) == EOF) {
      printf("\n Error : Fputs error\n");
  }

  sleep(2);

  message = GET("bla");
  printf("Sending %s\n", message);
  c_send_TLS(message);

  n = c_receive_TLS(buf);
  if(fputs(buf, stdout) == EOF) {
      printf("\n Error : Fputs error\n");
  }

  sleep(2);

  message = GET("unifr");
  printf("Sending %s\n", message);
  c_send_TLS(message);

  n = c_receive_TLS(buf);
  if(fputs(buf, stdout) == EOF) {
      printf("\n Error : Fputs error\n");
  }

  sleep(2);


  message = DEL("unifr");
  printf("Sending %s\n", message);
  c_send_TLS(message);

  n = c_receive_TLS(buf);
  if(fputs(buf, stdout) == EOF) {
      printf("\n Error : Fputs error\n");
  }


  if(!strcmp(buf, SUCCESS_DEL("myThirdkey", "OPisgreat!"))) {
    logger("All well, exiting now", INFO);
    c_end_TLS();
    logger("Socket closed", INFO);
    exit(0);
  } else {
    c_end_TLS();
    logger("guess we got a wrong message", ERROR);
  }

  return 0;
}
