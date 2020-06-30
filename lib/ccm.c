#include "ccm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wmmintrin.h>

#include "aes-intrinsics.h"
#include "aes.h"
#include "utils.h"

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
  __uint8_t flags = reserved | Adata | t_encoded | q_encoded;

  B[0] = flags;
  memcpy(&B[1], N, n);
  memcpy(&B[n + 1], Q, q);

  free(Q);

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
  *r = u + p_ceil;

  __uint8_t* B = (__uint8_t*)calloc(1 + *r, 16);
  memcpy(B, B0, 16);
  memcpy(&B[16], Bu, u * 16);
  /* Formatting payload */
  memcpy(&B[16 + u * 16], P, p);

  free(B0);
  free(Bu);
  return B;
}

void formatting_ctr(unsigned char* N, size_t n, size_t i, unsigned char* out) {
  if (n > 13) n = 13;
  __uint8_t q = 15 - n;

  __uint8_t reserved = 0 & 0b11111000; /* Bits to set in 0 */
  __uint8_t q_encoded = encode_x_in_s(q - 1, 3)[0];
  __uint8_t flags = reserved | q_encoded;
  __uint8_t* i_encoded = encode_x_in_s(i, 8 * q);
  // printf("i_encoded: %s\n", bytes_to_hex(i_encoded, q));

  out[0] = flags;
  memcpy(&out[1], N, n);
  memcpy(&out[n + 1], i_encoded, q);
  // printf("out: %s\n", bytes_to_hex(out, 16));

  free(i_encoded);
}

/**
 * The bit string consisting of the s right-most bits of the bit string X.
 */
void LSB(unsigned char* x, size_t lenX, size_t s, unsigned char* out) {
  __uint8_t s_residue = s % 8;
  size_t lsb_bytes_len =
      s_residue == 0 ? s / 8 : s / 8 + 1; /* Number of bytes */

  if (lsb_bytes_len > lenX) {
    /* TODO */
  }

  for (size_t i = 0; i < lsb_bytes_len; i++) {
    out[i] = x[lenX - lsb_bytes_len + i];
  }

  /* Remove left excess bits */
  if (s_residue > 0) {
    __uint8_t aux_residue_and_op = (1 << s_residue) - 1;
    out[0] &= aux_residue_and_op;
  }
}

/**
 * The bit string consisting of the s left-most bits of the bit string X.
 */
void MSB(unsigned char* x, size_t lenX, size_t s, unsigned char* out) {
  __uint8_t s_residue = s % 8;
  size_t msb_bytes_len =
      s_residue == 0 ? s / 8 : s / 8 + 1; /* Number of bytes */

  if (msb_bytes_len > lenX) {
    /* TODO */
  }

  for (size_t i = 0; i < msb_bytes_len; i++) {
    out[i] = x[i];
  }

  /* Remove left excess bits */
  if (s_residue > 0) {
    __uint8_t aux_residue_and_op = (1 << s_residue) - 1;
    aux_residue_and_op <<= 8 - aux_residue_and_op;
    out[0] &= aux_residue_and_op;
  }
}

// 1.Apply the formatting function to ( N , A , P ) to produce the blocks B 0 ,
// B 1 , ..., B r .
// 2.Set Y 0 = CIPH K ( B 0 ).
// 3.For i = 1 to r , do Y i = CIPH K ( B i ⊕ Y i-1 ).
// 4.Set T =MSB Tlen ( Y r ).
// 5.Apply the counter generation function to generate the counter blocks Ctr 0
// , Ctr 1 ,
// ..., Ctr m , where m = ⎡ Plen 128 ⎤ .
// 6.For j =0 to m , do S j = CIPH K ( Ctr j ).
// 7.Set S= S 1 || S 2 || ...|| S m .
// 8.Return C =( P ⊕ MSB Plen ( S )) || ( T ⊕ MSB Tlen ( S 0 )).
void ccm_encrypt(unsigned char* nonce, size_t n, unsigned char* A, size_t a,
                 unsigned char* P, size_t p, unsigned char* key, size_t Tlen,
                 unsigned char* out) {
  unsigned char key_expanded[16 * (ROUNDS + 1)];
  AES_128_Key_Expansion((const unsigned char*)key, key_expanded);
  // printf("A: %s\n", bytes_to_hex(A, a));
  // printf("K: %s\n", bytes_to_hex(key, 16));

  size_t r;
  size_t Plen = p * 8, t = Tlen % 8 == 0 ? Tlen / 8 : Tlen / 8 + 1;
  // printf("n:%d, a:%d, p:%d\n", n, a, p);

  // 1.Apply the formatting function to ( N , A , P ) to produce the blocks
  // B 0 , B 1 , ..., B r .
  unsigned char* b = formatting_input_data(nonce, n, A, a, P, p, t, &r);
  // printf("B: %s\n", bytes_to_hex(b, (1 + r) * 16));
  // printf("r: %d\n", r);

  unsigned char y[16 * (1 + r)];
  unsigned char ivec[16];
  memset(ivec, 0, 16);

  // 2.Set Y 0 = CIPH K ( B 0 ).
  AES_CBC_encrypt(b, y, ivec, 16, key_expanded, ROUNDS);

  // 3.For i = 1 to r , do Y i = CIPH K ( B i ⊕ Y i-1 ).
  for (size_t i = 1; i <= r; i++) {
    AES_CBC_encrypt(&b[16 * i], &y[16 * i], &y[16 * (i - 1)], 16, key_expanded,
                    ROUNDS);
  }

  // 4.Set T =MSB Tlen ( Y r ). la cadena de tamaño Tlen de los bits mas
  // significativos izquierdos de Yr
  unsigned char T[t];
  MSB(&y[r * 16], 16, Tlen, T);
  // printf("T: %s\n", bytes_to_hex(&T[0], t));

  //  5.Apply the counter generation function to generate the counter blocks Ctr
  //  0 , Ctr 1 ,
  // ..., Ctr m , where m = ⎡ Plen 128 ⎤
  size_t m = (size_t)ceil(Plen / 128.0);
  __uint8_t ctr[16 * (1 + m)];

  for (size_t i = 0; i <= m; i++) {
    formatting_ctr(nonce, n, i, &ctr[16 * i]);
  }
  // printf("Ctr0: %s\n", bytes_to_hex(&ctr[0], 16));

  // 6. For j =0 to m , do S j = CIPH K ( Ctr j ).
  // 7. Set S= S 1 || S 2 || ...|| S m .
  __uint8_t s0[16];
  AES_CBC_encrypt(&ctr[0], s0, ivec, 16, key_expanded, ROUNDS);
  // printf("S0: %s\n", bytes_to_hex(s0, 16));

  __uint8_t s[16 * m];
  for (size_t i = 1; i <= m; i++) {
    AES_CBC_encrypt(&ctr[16 * i], &s[16 * (i - 1)], ivec, 16, key_expanded,
                    ROUNDS);
  }
  // printf("ctr1: %s\n", bytes_to_hex(&ctr[16* 1], 16));
  // printf("S1: %s\n", bytes_to_hex(&s[0], 16));

  // 8. Return C =( P xor MSB Plen ( S )) || ( T xor MSB Tlen ( S 0 )).
  __uint8_t msb_s[p];
  __uint8_t msb_s0[t];

  MSB(s, 16 * m, Plen, &msb_s[0]);
  MSB(s0, 16, Tlen, &msb_s0[0]);

  xor(P, msb_s, Plen, &out[0]);
  xor(T, msb_s0, Tlen, &out[p]);

  free(b);
}

void ccm_decrypt(unsigned char* nonce, size_t n, unsigned char* A, size_t a,
                 unsigned char* P, size_t p, unsigned char* key, size_t Tlen,
                 unsigned char* out) {
  // TODO
}