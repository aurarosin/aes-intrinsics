#include <stdio.h>

#include "aes.h"

int main() {
  // 3243f6a8885a308d313198a2e0370734
  char* text = "Hola";
  //char* text = (char*) hex_to_bytes("3243f6a8885a308d313198a2e0370734");
  // 2b7e151628aed2a6abf7158809cf4f3c
  char* key = "1234567890123456";
  //char* key = (char*) hex_to_bytes("2b7e151628aed2a6abf7158809cf4f3c");

  printf("key: %s\n",bytes_to_hex(key,16));
  char* encrypt = text_encrypt(text, key);
  char* decrypt = text_decrypt(encrypt, key);

  printf("Text: %s\n", text);
  printf("Encrypt: %s\n", encrypt);
  printf("Decrypt: %s\n", decrypt);
  // 3925841d02dc09fbdc118597196a0b32

  return 0;
}
