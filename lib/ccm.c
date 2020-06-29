#include "ccm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Encode a number x in s bits.
 */
unsigned char* encode_x_in_s(size_t x, size_t s) {
  __uint8_t s_residue = s % 8;
  size_t encode_len = s_residue == 0 ? s / 8 : s / 8 + 1; /* Number of bytes */
  unsigned char* encode = (unsigned char*)calloc(encode_len, 1);

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

unsigned char* formatting_ctrl_inf_and_nonce(unsigned char* N, size_t n,
                                             size_t a, size_t p, size_t t) {
  unsigned char* B = (unsigned char*)malloc(16);

  if (n > 13) n = 13;
  __uint8_t q = 15 - n;
  __uint8_t* Q = encode_x_in_s(p, 8 * q);

  __uint8_t reserved = 0;
  __uint8_t Adata = a > 0 ? ADATA_ON : 0;
  __uint8_t t_encoded = encode_x_in_s(((t - 2) / 2), 3)[0] << 3;
  __uint8_t q_encoded = encode_x_in_s(q - 1, 3)[0];

  B[0] = reserved | Adata | t_encoded | q_encoded;
  memcpy(&B[1], N, n);
  memcpy(&B[n + 1], Q, q);

  return B;
}

unsigned char* formatting_associated_data(unsigned char* B0, unsigned char* A,
                                          size_t a, size_t* blocks_len) {
  __uint8_t Adata = B0[0] & ADATA_ON;
  __uint8_t* encoded = NULL;
  __uint8_t a_encoded_len = 0;

  /* Encode value 'a' */
  if (Adata > 0) {
    if (a < 65280) {  // 2^16 - 2^8
      encoded = encode_x_in_s(a, 16);
      a_encoded_len = 2;
    } else if (a >= 65280 && a < 4294967296L) {  // 2^32
      __uint8_t* a_encoded = encode_x_in_s(a, 32);
      a_encoded_len = 6;
      encoded = (__uint8_t*)malloc(a_encoded_len);

      encoded[0] = 0xff;
      encoded[1] = 0xfe;

      for (__uint8_t i = 0; i < 4; i++) {
        encoded[2 + i] = a_encoded[i];
      }
    } else {
      __uint8_t* a_encoded = encode_x_in_s(a, 64);
      a_encoded_len = 10;
      encoded = (__uint8_t*)malloc(a_encoded_len);

      encoded[0] = 0xff;
      encoded[1] = 0xff;

      for (__uint8_t i = 0; i < 8; i++) {
        encoded[2 + i] = a_encoded[i];
      }
    }
  }

  /* Copy associated data A */
  encoded = realloc(encoded, a_encoded_len + a);
  memcpy(&encoded[a_encoded_len], A, a);

  /* Padding 0's */
  __uint8_t block_residue = 16 - (a_encoded_len + a) % 16;
  if (block_residue > 0) {
    encoded = realloc(encoded, a_encoded_len + a + block_residue);
    memset(&encoded[a_encoded_len + a], 0, block_residue);
  }

  *blocks_len = (a_encoded_len + a + block_residue) / 16;
  printf("DEBUG: %lu\n", *blocks_len);

  return encoded;
}

/**
 * @param n Is an element of {7, 8, 9, 10, 11, 12, 13} and n+q=15.
 * @param a < 264.
 * @param t Is an element of {4, 6, 8, 10, 12, 14, 16}.
 * @param r Update with the number of total blocks(16 bits each) in formatting
 * message return.
 */
unsigned char* formatting_input_data(unsigned char* N, size_t n,
                                     unsigned char* A, size_t a,
                                     unsigned char* P, size_t p, size_t t,
                                     size_t* r) {
  __uint8_t* B0 = formatting_ctrl_inf_and_nonce(N, n, a, p, t);
  size_t u;
  __uint8_t* Bu = formatting_associated_data(B0, A, a, &u);

  size_t p_ceil = (int)ceil(p / 16.0);
  *r = 1 + u + p_ceil;
  printf("DEBUG: %lu %lu\n", u, p_ceil);
  __uint8_t* B = (__uint8_t*)calloc(*r, 16);
  memcpy(B, B0, 16);
  memcpy(&B[16], Bu, u * 16);
  /* Formatting payload */
  memcpy(&B[16 + u * 16], P, p);

  free(B0);
  free(Bu);

  return B;
}
