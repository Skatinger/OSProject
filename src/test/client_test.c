#include <unistd.h>
#include "../client/client.h"
#include "../client/clientRequests.h"

int main(int argc, char *argv[]) {
  if(argc != 2) {
      printf("\n Usage: %s <ip of server> \n",argv[0]);
      return 1;
  }

  c_connect_TLS(argv[1]);

  // write a simple request
  char* message = LOGIN("unifr", "OPisgreat");

  sleep(5); // wait until message is sent to give some time to the testing
             // programmer
             //
  printf("Sending %s\n", message);
  c_send_TLS(message);
  char buf[BUFFER_SIZE];
  int n = c_receive_TLS(buf);

  if(fputs(buf, stdout) == EOF) {
      printf("\n Error : Fputs error\n");
  }

  if(!strcmp(buf, SUCCESS_LOGIN("unifr"))) {
    printf("All well, exiting now\n");
    c_end_TLS();
    printf("Socket closed\n");
    exit(0);
  } else {
    printf("guess we got a wrong message\n");
  }

  return 0;
}
