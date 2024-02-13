#include "SerialFi.h"

const char* charptr(const __FlashStringHelper * str) {
  int len = strlen_P((PGM_P)str);
  char* ptr = new char[len+1];
  strcpy_P(ptr, (PGM_P)str);
  return ptr;
}
