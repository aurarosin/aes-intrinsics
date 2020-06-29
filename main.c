#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  size_t n = 8, a = 5, p = 17409;
  size_t t = 12, r;
  unsigned char N[] = {0b00010011, 0b11010100, 0b10100011, 0b01011101,
                        0b01110001, 0b10100101, 0b00000000, 0b00000000};
  unsigned char A[a];
  unsigned char P[p];
  memset(A, 0xaa, a);
  memset(P, 0xbb, p);

  __uint8_t* formatting_input = formatting_input_data(N, n, A, a, P, p, t, &r);
  for (size_t i = 0; i < 16 * r; i++) {
    printf("%02x", formatting_input[i]);
  }
  printf("\n");

  free(formatting_input);
}

int main() {
  test_aes_cbc();
  test_ccm_formatting();

  printf("TQ, wait for me.");

  return 0;
}
