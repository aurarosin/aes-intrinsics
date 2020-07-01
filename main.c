#include <math.h>
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
  unsigned char N[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                       0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b};
  unsigned char A[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                       0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13};
  unsigned char P[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                       0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  size_t Tlen = 64, t = ceill(Tlen / 8.0);
  size_t n = 96 / 8, a = 160 / 8, Plen = 192, p = ceill(Plen / 8.0);
  size_t c = p + t;
  __uint8_t C[c], P_decrypt[p];

  ccm_encrypt(N, n, A, a, P, p, K, Tlen, C);
  printf("C: %s\n", bytes_to_hex(C, c));

  if (ccm_decrypt(N, n, A, a, C, c, K, Tlen, P_decrypt) == EXIT_SUCCESS) {
    printf("P decrypt: %s\n", bytes_to_hex(P_decrypt, p));
  } else {
    printf("P decrypt: INVALID\n");
  }
}

int main() {
  // test_aes_cbc();
  // test_ccm_formatting();
  test_ccm();

  return 0;
}
