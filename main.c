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
  for (size_t i = 0; i < 16 * (1 + r); i++) {
    printf("%02x", formatting_input[i]);
  }
  printf("\n");

  free(formatting_input);
}

void test_ccm() {
  unsigned char K[] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                       0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f};
  unsigned char N[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
  unsigned char A[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  unsigned char P[] = {0x20, 0x21, 0x22, 0x23};
  size_t Tlen = 32, Plen = 32, n = 56 / 8, a = 64 / 8, p = Plen / 8;

  __uint8_t c[Plen + Tlen];
  ccm_encrypt(N, n, A, a, P, p, K, Tlen, c);

  printf("C: %s\n", bytes_to_hex(c, p + Tlen / 8));
}

int main() {
  // test_aes_cbc();
  // test_ccm_formatting();
  test_ccm();

  printf("Te quiero.\n");

  return 0;
}
