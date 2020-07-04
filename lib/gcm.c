#include "gcm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes-intrinsics.h"
#include "gcm-intrinsics.h"
#include "aes.h"
#include "utils.h"

void gcm_encrypt(unsigned char* P, size_t p, unsigned char* A, size_t a,
                 unsigned char* IV, size_t iv, unsigned char* key,
                 unsigned char* out) {
  size_t h = 16;
  __uint8_t H[h];
  __uint8_t zeros[h];
  __uint8_t keyExpanded[16 * (1 + ROUNDS)];

  memset(zeros, 0, h);
  AES_128_Key_Expansion(key, keyExpanded);

  // 1. Let H = CIPH K (0 128 ).
  AES_CBC_encrypt(zeros, H, zeros, h, keyExpanded, ROUNDS);

  // 2. Define a block, J0
  __uint8_t J0[iv + 4];

  if (bits_len(iv) == 96) {
    memcpy(J0, IV, iv);
    memcpy(&J0[iv], 0, 31);
    J0[iv + 31] = 1;
  } else {
    size_t s = 128 * ceill(bits_len(iv) / 128.0) - bits_len(iv);
    // TODO: size_t x = iv + bytes_len(s+64) + ;
    //__uint8_t X[x];
    //GHASH(X, x, H, h, J0);
  }

  // TODO: All implementation...
}

int gcm_decrypt(unsigned char* nonce, size_t n, unsigned char* A, size_t a,
                unsigned char* C, size_t c, unsigned char* key, size_t Tlen,
                unsigned char* out_P) {}