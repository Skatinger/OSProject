#ifndef SERVER_RESPONSES
#define SERVER_RESPONSES
#include <string.h>

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


#endif
