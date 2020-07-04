#include "gcm.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes-intrinsics.h"
#include "aes.h"
#include "gcm-intrinsics.h"
#include "utils.h"

void gcm_encrypt(unsigned char* P, size_t p, unsigned char* A, size_t a,
                 unsigned char* IV, size_t iv, unsigned char* key,
                 unsigned char* T, unsigned char* out) {
  unsigned char key_expanded[16 * (ROUNDS + 1)];

  AES_128_Key_Expansion((const unsigned char*)key, key_expanded);
  AES_GCM_encrypt(P, out, A, IV, T, p, a, iv, key_expanded, ROUNDS);
}

int gcm_decrypt(unsigned char* C, size_t c, unsigned char* A, size_t a,
                unsigned char* IV, size_t iv, unsigned char* key,
                unsigned char* T, unsigned char* out) {
  unsigned char key_expanded[16 * (ROUNDS + 1)];

  AES_128_Key_Expansion((const unsigned char*)key, key_expanded);
  AES_GCM_decrypt(C, out, A, IV, T, c, a, iv, key_expanded, ROUNDS);
}