#ifndef SERVER_RESPONSES
#define SERVER_RESPONSES

#include <stdio.h>
#include <stdlib.h>
#include "../utils/string_stuff.h"
#include "../project.h"


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
#define ERROR_ALREADY_LOGGEDIN_NR 501
#define ERROR_LOGOUT_FAIL_NR      600
#define ERROR_USER_OCCUPIED_NR    700
#define ERROR_USERNAME_INVALID_NR 701
#define ERROR_USER_MODIFICATION_NR 702
#define ERROR_USER_NOT_UPDATABLE_NR  703
#define ERROR_NO_ADMIN_NR         777
#define ERROR_SERVER_FULL_NR      999

/* ============ SUCCESS MESSAGES ================ */
#define SUCCESS_GOT(K,V) concatCode(SUCCESS_GOT_NR, 5, " GOT ", K, ":", V, ";\n")
#define SUCCESS_PUT(K,V) concatCode(SUCCESS_PUT_NR, 5, " PUT ", K, ":", V, ";\n")
#define SUCCESS_DEL(K,V) concatCode(SUCCESS_DEL_NR, 5, " DELETED ", K, ":", V, ";\n")
#define SUCCESS_UPD(K,V) concatCode(SUCCESS_UPD_NR, 5, " UPDATED ", K, ":", V, ";\n")
#define SUCCESS_LOGIN(U) concatCode(SUCCESS_LOGIN_NR, 3, " USER ", U, " LOGGED IN;\n")
#define SUCCESS_LOGOUT  concatCode(SUCCESS_LOGOUT_NR, 1, " LOGOUT SUCCESSFUL;\n")
#define SUCCESS_ADD_U(U) concatCode(SUCCESS_ADD_U_NR, 3, " USER ", U, " ADDED;\n")
#define SUCCESS_DEL_U(U) concatCode(SUCCESS_DEL_U_NR, 3, " USER ", U, " DELETED;\n")
#define SUCCESS_CHG_U(U1, U2) concatCode(SUCCESS_CHG_U_NR, 5, " USER ", U1, " UPDATED TO ", U2, " AND NEW PASSWORD SET;\n")
#define SUCCESS_MK_ADM(U) concatCode(SUCCESS_MK_ADM_NR, 3, " USER ", U, " IS NOW ADMIN;\n")

/* ============== ERROR MESSAGES =============== */
#define ERROR_KEY_NOT_FOUND(K) concatCode(ERROR_KEY_NOT_FOUND_NR, 3, " KEY ", K, " NOT FOUND;\n")
#define ERROR_KEY_INVALID concatCode(ERROR_KEY_INVALID_NR, 1, " INVALID KEY;\n")
#define ERROR_VALUE_INVALID concatCode(ERROR_VALUE_INVALID_NR, 1, " INVALID VALUE;\n")
#define ERROR_KEY_OCCUPIED(K) concatCode(ERROR_KEY_OCCUPIED_NR, 3, " KEY ", K, " NOT FREE;\n")
#define ERROR_ACCESS_DENIED(U) concatCode(ERROR_ACCESS_DENIED_NR, 3, " ACCESS FOR USER ", U, " DENIED;\n")
#define ERROR_ALREADY_LOGGEDIN(U) concatCode(ERROR_ALREADY_LOGGEDIN_NR, 3, " USER ", U, " ALREADY LOGGED IN;\n")
#define ERROR_LOGOUT_FAIL concatCode(ERROR_LOGOUT_FAIL_NR, 1, " LOGOUT FAILED;\n")
#define ERROR_USER_OCCUPIED(U) concatCode(ERROR_USER_OCCUPIED_NR, 3, " USERNAME ", U, " NOT FREE;\n")
#define ERROR_USERNAME_INVALID concatCode(ERROR_USERNAME_INVALID_NR, 1, " INVALID USERNAME;\n")
#define ERROR_USER_MODIFICATION concatCode(ERROR_USER_MODIFICATION_NR, 1, " USER MODIFICATION FAILED;\n")
#define ERROR_USER_NOT_UPDATABLE(U) concatCode(ERROR_USER_NOT_UPDATABLE_NR, 3, " USER ", U, " CANNOT BE UPDATED;\n");
#define ERROR_NO_ADMIN(U) concatCode(ERROR_NO_ADMIN_NR, 3, " USER ", U, " HAS NO ADMIN RIGHTS;\n");
#define ERROR_SERVER_FULL concatCode(ERROR_SERVER_FULL_NR, 1, " SERVER IS FULL;\n")

/* ============= OTHERS ======================== */
#ifndef BYE
  #define BYE "BYE ;\n"
#endif


#endif
