#include <stdio.h>
#include <stdlib.h>

#include "aes.h"
#include "ccm.h"

void test_aes_cbc() {
  char* text = "Hola";
  // char* text = (char*) hex_to_bytes("3243f6a8885a308d313198a2e0370734");
  char* key = "1234567890123456";
  // char* key = (char*) hex_to_bytes("2b7e151628aed2a6abf7158809cf4f3c");

  char* encrypt = text_encrypt(text, key);
  char* decrypt = text_decrypt(encrypt, key);

  printf("Text: %s\n", text);
  printf("Encrypt: %s\n", encrypt);
  printf("Decrypt: %s\n", decrypt);
  // 3925841d02dc09fbdc118597196a0b32

  free(encrypt);
  free(decrypt);
}

void test_ccm_formatting() {
  unsigned char N[8] = {0b00010011, 0b11010100, 0b10100011, 0b01011101, 0b01110001, 0b10100101, 0b00000000, 0b00000000};
  
  __uint8_t* formatt = formatting(N, 8, NULL, 1, NULL, 17409, 12);
  for (size_t i = 0; i < 16; i++)
  {
    printf("%02x", formatt[i]);
  }
  printf("\n");
  free(formatt);
}

int main() {
  
  test_aes_cbc();
  test_ccm_formatting();

  return 0;
}
