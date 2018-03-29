#ifndef SERVER_RESPONSES
#define SERVER_RESPONSES
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* =========SUCCESS CODE DEFS======================*/
#define SUCCESS_GOT_NR    10
#define SUCCESS_PUT_NR    20
#define SUCCESS_DEL_NR    30
#define SUCCESS_UPD_NR    40
#define SUCCESS_LOGIN_NR  50
#define SUCCESS_LOGOUT_NR 60
#define SUCCESS_ADD_U_NR  70
#define SUCCESS_DEL_U_NR  80
#define SUCCESS_CHG_U_NR  90
#define SUCCESS_MK_ADM_NR 95

/* ========ERROR CODE DEFS=========================*/
#define ERROR_KEY_NOT_FOUND_NR    100
#define ERROR_KEY_INVALID_NR      201
#define ERROR_VALUE_INVALID_NR    202
#define ERROR_KEY_OCCUPIED_NR     203
#define ERROR_ACCESS_DENIED_NR    500
#define ERROR_LOGOUT_FAIL_NR      600
#define ERROR_USER_OCCUPIED_NR    700
#define ERROR_USERNAME_INVALID_NR 701
#define ERROR_NO_ADMIN_MR         777
#define ERROR_SERVER_FULL_NR      999

/* ============ SUCCESS MESSAGES ================ */
#define SUCCESS_GOT(K,V) concatCode(SUCCESS_GOT_NR, 5, " GOT ", K, ":", V, ";")
#define SUCCESS_PUT(K,V) concatCode(SUCCESS_PUT_NR, 5, " PUT ", K, ":", V, ";")
#define SUCCESS_DEL(K,V) concatCode(SUCCESS_DEL_NR, 5, " DELETED ", K, ":", V, ";")
#define SUCCESS_UPD(K,V) concatCode(SUCCESS_UPD_NR, 5, " UDATED ", K, ":", V, ";")
#define SUCCESS_LOGIN(U) concatCode(SUCCESS_LOGIN_NR, 3, " USER ", U, " LOGGED IN;")
#define SUCCESS_LOGOUT  concatCode(SUCCESS_LOGOUT_NR, 1, " LOGOUT SUCCESSFUL;")
#define SUCCESS_ADD_U(U) concatCode(SUCCESS_ADD_U_NR, 3, " USER ", U, " ADDED;")
#define SUCCESS_DEL_U(U) concatCode(SUCCESS_DEL_U_NR, 3, " USER ", U, " DELETED;")
#define SUCCESS_CHG_U(U1, U2) concatCode(SUCCESS_CHG_U_NR, 5, " USER ", U1, " UPDATED TO ", U2, " AND NEW PASSWORD SET;")
#define SUCCESS_MK_ADM(U) concatCode(SUCCESS_MK_ADM_NR, 3, " USER ", U, " IS NOW ADMIN;")

/* ============== ERROR MESSAGES =============== */
#define ERROR_KEY_NOT_FOUND(K) concatCode(ERROR_KEY_NOT_FOUND_NR, 3, " KEY ", K, " NOT FOUND;")
#define ERROR_KEY_INVALID concatCode(ERROR_KEY_INVALID_NR, 1, " INVALID KEY;")
#define ERROR_VALUE_INVALID concatCode(ERROR_VALUE_INVALID_NR, 1, " INVALID VALUE;")
#define ERROR_KEY_OCCUPIED(K) concatCode(ERROR_KEY_OCCUPIED_NR, 3, " KEY ", K, " NOT FREE;")
#define ERROR_ACCESS_DENIED(U) concatCode(ERROR_ACCESS_DENIED_NR, 3, " ACCESS FOR USER ", U, " DENIED;")
#define ERROR_LOGOUT_FAIL concatCode(ERROR_LOGOUT_FAIL_NR, 1, " LOGOUT FAILED;")
#define ERROR_USER_OCCUPIED(U) concatCode(ERROR_USER_OCCUPIED_NR, 3, " USERNAME ", U, " NOT FREE;")
#define ERROR_USERNAME_INVALID concatCode(ERROR_USERNAME_INVALID_NR, 1, " INVALID USERNAME;")
#define ERROR_NO_ADMIN(U) concatCode(ERROR_NO_ADMIN, 3, " USER ", U, " HAS NO ADMIN RIGHTS");
#define ERROR_SERVER_FULL concatCode(ERROR_SERVER_FULL_NR, 1, " SERVER IS FUL")

char* concat(int num, ...) {
  va_list vlist;
  va_start(vlist, num);
  char* dest = va_arg(vlist, char*);
  for (int i = 1; i < num; i++) {
    char* source = va_arg(vlist, char*);
    strcat(dest,source);
  }
  va_end(vlist);
  return dest;
}

char* intToString(int num) {
  char* ret = malloc(100*sizeof(char));
  sprintf(ret, "%d", num);
  return ret;
}

char* concatCode(int code, int nbWords, ...) {
  va_list vlist;
  va_start(vlist, nbWords);
  char* dest = intToString(code);
  for (int i = 0; i < nbWords; i++) {
    char* source = va_arg(vlist, char*);
    strcat(dest,source);
  }
  va_end(vlist);
  return dest;
}

#endif
