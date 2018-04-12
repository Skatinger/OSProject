#ifndef CLIENT_REQs
#define CLIENT_REQs

#include <string.h>
#include <stdarg.h>
#include "../server/serverResponses.h"
#include "../project.h"

#define GET(K) concat(3, "GET ", K, ";\n")
#define PUT(K,V) concat(5, "PUT ", K, ":", V, ";\n")
#define DEL(K) concat(3, "DEL ", K, ";\n")
#define UPD(K,V) concat(5, "UPD ", K, ":", V, ";\n")
#define LOGIN(U,P) concat(5, "LOGIN ", U, ":", P, ";\n")
#define LOGOUT(U) concat(3, "LOGOUT ", U, ";\n")
#define ADD_U(U,P) concat(5, "ADD_U ", U, ":", P, ";\n")
#define DEL_U(U) concat(3, "DEL_U ", U, ";\n")
#define CHG_U(U1,U2,P) concat(7, "CHG_U ", U1, ":", U2, ":", P, ";\n")
#define MK_ADMIN(U) concat(3, "MK_ADM ", U, ";\n")
#define BYE "BYE;\n";



#endif
