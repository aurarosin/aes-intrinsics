#include "ccm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Encode a number x in s bits.
 */
unsigned char* encode_x_in_s(size_t x, size_t s) {
  __uint8_t s_residue = s % 8;
  size_t encode_len = s_residue == 0 ? s / 8 : s / 8 + 1; /* Number of bytes */
  unsigned char* encode = (unsigned char*)malloc(encode_len);
  memset(encode, 0, encode_len);

  /* Copy bytes of x number into a bytes array */
  __uint8_t x_bytes_len = encode_len > 8 ? 8 : encode_len;
  __uint8_t* x_bytes = (__uint8_t*)malloc(x_bytes_len);
  memcpy(x_bytes, &x, x_bytes_len);

  /* Remove excess bits */
  if (s_residue != 0) {
    __uint8_t aux_residue_and_op = (1 << s_residue) - 1;
    x_bytes[x_bytes_len - 1] = x_bytes[x_bytes_len - 1] & aux_residue_and_op;
  }

  /* Copy in reverse bytes array of x number */
  for (__uint8_t i = 0; i < x_bytes_len; i++) {
    encode[encode_len - 1 - i] = x_bytes[i];
  }

  free(x_bytes);
  return encode;
}

/**
 * @param n Is an element of {7, 8, 9, 10, 11, 12, 13} and n+q=15.
 * @param a < 264.
 * @param t Is an element of {4, 6, 8, 10, 12, 14, 16}.
 */
unsigned char* formatting(unsigned char* N, size_t n, unsigned char* A,
                          size_t a, unsigned char* P, size_t p, size_t t) {
  unsigned char* B = (unsigned char*)malloc(16);
  if (n > 13) n = 13;
  __uint8_t q = 15 - n;
  __uint8_t* Q = encode_x_in_s(p, 8 * q);

  __uint8_t Adata = a > 0 ? ADATA_ON : 0;
  __uint8_t t_encoded = encode_x_in_s(((t - 2) / 2), 3)[0] << 3;
  __uint8_t q_encoded = encode_x_in_s(q - 1, 3)[0];

  B[0] = RESERVED | Adata | t_encoded | q_encoded;
  memcpy(&B[1], N, n);
  memcpy(&B[n + 1], Q, q);

  return B;
}