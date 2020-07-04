#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes.h"
#include "ccm.h"
#include "gcm.h"
#include "utils.h"

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

  printf("K: %s\n", bytes_to_hex(K, 16));
  printf("N: %s\n", bytes_to_hex(N, n));
  printf("A: %s\n", bytes_to_hex(A, a));
  printf("P: %s\n", bytes_to_hex(P, p));

  struct timeval t1_e = get_timestamp();
  ccm_encrypt(N, n, A, a, P, p, K, Tlen, C);
  struct timeval t2_e = get_timestamp();

  struct timeval t1_d = get_timestamp();
  int dec_status = ccm_decrypt(N, n, A, a, C, c, K, Tlen, P_decrypt);
  struct timeval t2_d = get_timestamp();

  printf("C encrypt: %s\n", bytes_to_hex(C, c));

  if (dec_status == EXIT_SUCCESS) {
    printf("P decrypt: %s\n", bytes_to_hex(P_decrypt, p));
  } else {
    printf("P decrypt: INVALID\n");
  }

  printf("\nTime encrypt: %f ms.\n", (t2_e.tv_usec - t1_e.tv_usec) / 1000.0);
  printf("Time decrypt: %f ms.\n", (t2_d.tv_usec - t1_d.tv_usec) / 1000.0);
}

void test_gcm() {
  unsigned char K[] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                       0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f};
  unsigned char A[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                       0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13};
  unsigned char P[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                       0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  unsigned char IV[] = {34, 15,  20, 79,  33,  7, 1,  99,
                        58, 109, 12, 218, 172, 4, 86, 42};
  size_t n = 96 / 8, a = 160 / 8, Plen = 192, p = ceill(Plen / 8.0);
  size_t iv = 16;
  size_t c = p;
  __uint8_t C[c], P_decrypt[p];
  __uint8_t T[16];

  printf("K: %s\n", bytes_to_hex(K, 16));
  printf("A: %s\n", bytes_to_hex(A, a));
  printf("P: %s\n", bytes_to_hex(P, p));
  printf("IV: %s\n", bytes_to_hex(IV, iv));

  struct timeval t1_e = get_timestamp();
  gcm_encrypt(P, p, A, a, IV, iv, K, T, C);
  struct timeval t2_e = get_timestamp();

  struct timeval t1_d = get_timestamp();
  int dec_status = gcm_decrypt(C, c, A, a, IV, iv, K, T, P_decrypt);
  struct timeval t2_d = get_timestamp();

  printf("C encrypt: %s\n", bytes_to_hex(C, c));
  printf("Tag: %s\n", bytes_to_hex(T, 16));

  if (dec_status == 1) {
    printf("P decrypt: %s\n", bytes_to_hex(P_decrypt, p));
  } else {
    printf("P decrypt: INVALID\n");
  }

  printf("\nTime encrypt: %f ms.\n", (t2_e.tv_usec - t1_e.tv_usec) / 1000.0);
  printf("Time decrypt: %f ms.\n", (t2_d.tv_usec - t1_d.tv_usec) / 1000.0);
}

int main() {
  // test_aes_cbc();

  printf("\n>>>>> CCM\n");
  test_ccm();

  printf("\n>>>>> GCM\n");
  test_gcm();

  return 0;
}
