#ifndef CLIENT_REQs
#define CLIENT_REQs

#include <string.h>
#include <stdarg.h>
#include "../server/server_responses.h"
#include "../project.h"

#define GET(K) concat(3, "GET ", K, ";")
#define PUT(K,V) concat(5, "PUT ", K, ":", V, ";")
#define DEL(K) concat(3, "DEL ", K, ";")
#define UPD(K,V) concat(5, "UPD ", K, ":", V, ";")
#define KEY(V) concat(3, "KEY ", V, ";")
#define LOGIN(U,P) concat(5, "LOGIN ", U, ":", P, ";")
#define LOGOUT(U) concat(3, "LOGOUT ", U, ";")
#define ADD_U(U,P) concat(5, "ADD_U ", U, ":", P, ";")
#define DEL_U(U) concat(3, "DEL_U ", U, ";")
#define CHG_U(U1,U2,P) concat(7, "CHG_U ", U1, ":", U2, ":", P, ";")
#define MK_ADMIN(U) concat(3, "MK_ADM ", U, ";")
#ifndef BYE
  #define BYE "BYE ;\n";
#endif


#endif
