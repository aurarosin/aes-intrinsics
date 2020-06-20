#include <stdio.h>

#include "aes.h"

int main() {
  //3243f6a8885a308d313198a2e0370734
  unsigned char* text = "hello";
  //2b7e151628aed2a6abf7158809cf4f3c
  unsigned char* key = "1234567890123456";
  
  char* encrypt = text_encrypt(text, key);

  printf("Encrypt: %s\n", encrypt);
  //3925841d02dc09fbdc118597196a0b32

  // Encrypt: 92E9BB5C83C5DAF00CE9136544A90AA5
  757ccd0cdc5c90eadbeeecf638dd0000
  757CCD0CDC5C90EADBEEECF638DD0000 050187A0CDE5A9872CBAB091AB73E553

  return 0;
}
