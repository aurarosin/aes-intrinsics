#include "ccm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wmmintrin.h>

#include "aes-intrinsics.h"
#include "aes.h"
#include "utils.h"

unsigned char* formatting_ctrl_inf_and_nonce(unsigned char* N, size_t n,
                                             size_t a, size_t p, size_t t) {
  unsigned char* B = (unsigned char*)malloc(16);

  if (n > 13) n = 13;
  __uint8_t q = 15 - n;
  __uint8_t Q[q];
  encode_x_in_s(p, 8 * q, Q);

  __uint8_t reserved = 0;
  __uint8_t Adata = a > 0 ? ADATA_ON : 0;
  __uint8_t t_encoded;
  __uint8_t q_encoded;

  encode_x_in_s(((t - 2) / 2), 3, &t_encoded);
  t_encoded <<= 3;
  encode_x_in_s(q - 1, 3, &q_encoded);

  __uint8_t flags = reserved | Adata | t_encoded | q_encoded;

  B[0] = flags;
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
      encoded = (__uint8_t*)malloc(2);
      encode_x_in_s(a, 16, encoded);
      a_encoded_len = 2;
    } else if (a >= 65280 && a < 4294967296L) {  // 2^32
      __uint8_t a_encoded[4];
      encode_x_in_s(a, 32, a_encoded);
      a_encoded_len = 6;
      encoded = (__uint8_t*)malloc(a_encoded_len);

      encoded[0] = 0xff;
      encoded[1] = 0xfe;

      for (__uint8_t i = 0; i < 4; i++) {
        encoded[2 + i] = a_encoded[i];
      }
    } else {
      __uint8_t a_encoded[8];
      encode_x_in_s(a, 64, a_encoded);
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

  size_t p_ceil = ceill(p / 16.0);
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
  __uint8_t q_encoded;
  encode_x_in_s(q - 1, 3, &q_encoded);
  __uint8_t flags = reserved | q_encoded;
  __uint8_t i_encoded[q];
  encode_x_in_s(i, 8 * q, i_encoded);
  // printf("i_encoded: %s\n", bytes_to_hex(i_encoded, q));

  out[0] = flags;
  memcpy(&out[1], N, n);
  memcpy(&out[n + 1], i_encoded, q);
  // printf("out: %s\n", bytes_to_hex(out, 16));
}

void counter_generation(unsigned char* N, size_t n, size_t m,
                        unsigned char* out) {
  for (size_t i = 0; i <= m; i++) {
    formatting_ctr(N, n, i, &out[16 * i]);
  }
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
  size_t Plen = p * 8;
  size_t t = Tlen % 8 == 0 ? Tlen / 8 : Tlen / 8 + 1;
  size_t r;
  // printf("n:%d, a:%d, p:%d\n", n, a, p);

  unsigned char key_expanded[16 * (ROUNDS + 1)];
  AES_128_Key_Expansion((const unsigned char*)key, key_expanded);
  // printf("A: %s\n", bytes_to_hex(A, a));
  // printf("K: %s\n", bytes_to_hex(key, 16));

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
  //  0 , Ctr 1 ,..., Ctr m , where m = ⎡ Plen 128 ⎤
  size_t m = ceill(Plen / 128.0);
  __uint8_t ctr[16 * (1 + m)];
  counter_generation(nonce, n, m, ctr);
  // printf("Ctr0: %s\n", bytes_to_hex(&ctr[0], 16));

  // 6. For j =0 to m , do S j = CIPH K ( Ctr j ).
  // 7. Set S= S 1 || S 2 || ...|| S m .
  __uint8_t s0[16];
  __uint8_t s[16 * m];
  AES_CBC_encrypt(&ctr[0], s0, ivec, 16, key_expanded, ROUNDS);
  // printf("S0: %s\n", bytes_to_hex(s0, 16));
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

/**
 * 1. If Clen ≤ Tlen , then return INVALID.
 * 2. Apply the counter generation function to generate the counter blocks Ctr 0
 * , Ctr 1 ,..., Ctr m , where m = ⎡ ( Clen − Tlen ) 128 ⎤ .
 * 3. For j =0 to m , do S j = CIPH K ( Ctr j ).
 * 4. Set S= S 1 || S 2 || ...|| S m .
 * 5. Set P =MSB Clen-Tlen ( C ) ⊕ MSB Clen-Tlen ( S ).
 * 6. Set T =LSB Tlen ( C ) ⊕ MSB Tlen ( S 0 ).
 * 7. If N , A , or P is not valid, as discussed in Section 5.4, then return
 * INVALID, else apply the formatting function to ( N , A , P ) to produce the
 * blocks B 0 , B 1 , ..., B r .
 * 8. Set Y 0 = CIPH K ( B 0 ).
 * 9. For i = 1 to r , do Y j = CIPH K ( B i ⊕ Y i-1 ).
 * 10. If T ≠ MSB Tlen ( Y r ), then return INVALID, else return P .
 */
int ccm_decrypt(unsigned char* nonce, size_t n, unsigned char* A, size_t a,
                unsigned char* C, size_t c, unsigned char* key, size_t Tlen,
                unsigned char* out_P) {
  size_t Clen = 8 * c, t = ceill(Tlen / 8.0);
  unsigned char ivec[16];
  memset(ivec, 0, 16);

  // 1. If Clen ≤ Tlen , then return INVALID.
  if (Clen <= Tlen) {
    return EXIT_FAILURE;
  }

  unsigned char key_expanded[16 * (ROUNDS + 1)];
  AES_128_Key_Expansion((const unsigned char*)key, key_expanded);
  // printf("K: %s\n", bytes_to_hex(key, 16));

  // 2.Apply the counter generation function to generate the counter blocks Ctr
  // 0 , Ctr 1 , ..., Ctr m , where m = ⎡ ( Clen − Tlen ) 128 ⎤.
  size_t m = ceill((Clen - Tlen) / 128.0);
  __uint8_t ctr[16 * (1 + m)];
  counter_generation(nonce, n, m, ctr);
  // printf("ctr: %s\n", bytes_to_hex(ctr, 16 * (1 + m)));

  // 3. For j =0 to m , do S j = CIPH K ( Ctr j ).
  __uint8_t s0[16];
  __uint8_t s[16 * m];
  AES_CBC_encrypt(ctr, s0, ivec, 16, key_expanded, ROUNDS);
  // printf("S0: %s\n", bytes_to_hex(s0, 16));
  for (size_t i = 1; i <= m; i++) {
    AES_CBC_encrypt(&ctr[16 * i], &s[16 * (i - 1)], ivec, 16, key_expanded,
                    ROUNDS);
  }
  // printf("ctr1: %s\n", bytes_to_hex(&ctr[16* 1], 16));
  // printf("S1: %s\n", bytes_to_hex(&s[0], 16));

  // 5. Set P=MSB Clen-Tlen ( C ) xor MSB Clen-Tlen ( S ).
  size_t p = ceill((Clen - Tlen) / 8.0);
  __uint8_t msb_c[p];
  __uint8_t msb_s[p];
  MSB(C, c, Clen - Tlen, msb_c);
  MSB(s, 16 * m, Clen - Tlen, msb_s);
  xor(msb_c, msb_s, Clen - Tlen, out_P);

  // 6. Set T =LSB Tlen ( C ) xor MSB Tlen ( S 0 ).
  __uint8_t lsb_c[t];
  __uint8_t msb_s0[t];
  __uint8_t T[t];
  LSB(C, c, Tlen, lsb_c);
  MSB(s0, 16, Tlen, msb_s0);
  xor(lsb_c, msb_s0, Tlen, T);

  // 7. If N , A , or P is not valid, as discussed in Section 5.4, then return
  // INVALID, else apply the formatting function to ( N , A , P ) to produce the
  // blocks B 0 , B 1 , ..., B r .
  size_t r;
  __uint8_t* B;
  if ((n >= 7 && n <= 13)) {
    B = formatting_input_data(nonce, n, A, a, out_P, p, t, &r);
  } else {
    return EXIT_FAILURE;
  }

  // 8. Set Y 0 = CIPH K ( B 0 ).
  __uint8_t Y[16 * (1 + r)];
  AES_CBC_encrypt(&B[0], &Y[0], ivec, 16, key_expanded, ROUNDS);

  // 9. For i = 1 to r , do Y j = CIPH K ( B i ⊕ Y i-1 ).
  for (size_t i = 1; i <= r; i++) {
    AES_CBC_encrypt(&B[16 * i], &Y[16 * i], &Y[16 * (i - 1)], 16, key_expanded,
                    ROUNDS);
  }

  // 10. If T ≠ MSB Tlen ( Y r ), then return INVALID, else return P .
  __uint8_t msb_yr[t];
  MSB(&Y[16 * r], 16, Tlen, msb_yr);
  for (size_t i = 0; i < t; i++) {
    if (T[i] != msb_yr[i]) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}