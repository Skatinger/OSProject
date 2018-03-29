#include "../server/serverResponses.h"
#include <stdio.h>

int main() {
  char* K = "testKey?";
  char* V = "testValue!";
  char* U1 = "remog";
  char* U2 = "remogeissbuehler";

  printf("%s\n", SUCCESS_GOT(K, V));
  printf("%s\n", SUCCESS_GOT(K,V));
  printf("%s\n", SUCCESS_PUT(K,V));
  printf("%s\n", SUCCESS_DEL(K,V)) ;
  printf("%s\n", SUCCESS_UPD(K,V));
  printf("%s\n", SUCCESS_LOGIN(U1)) ;
  printf("%s\n", SUCCESS_LOGOUT);
  printf("%s\n", SUCCESS_ADD_U(U2)) ;
  printf("%s\n", SUCCESS_DEL_U(U1)) ;
  printf("%s\n", SUCCESS_CHG_U(U1, U2)) ;
  printf("%s\n", SUCCESS_MK_ADM(U2));

  exit(0);
}
