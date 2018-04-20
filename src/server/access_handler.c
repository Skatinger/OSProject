#include "serverResponses.h"
#include "access_handler.h"

char* reader(char* key) {
  return SUCCESS_GOT(key, "someValue");
}

char* writer(char* key, char* value, int type) {
  switch (type) {
    case PUT:
      return SUCCESS_PUT(key, "someValue");
      break;
    case UPD:
      return SUCCESS_UPD(key, "someNewValue");
      break;
    case DEL:
      return SUCCESS_DEL(key, "someValue");
      break;
  }
}
