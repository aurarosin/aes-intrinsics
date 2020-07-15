#include "sha256.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint32_t B2U32(uint8_t val, uint8_t sh) { return ((uint32_t)val) << sh; }

void sha256_process(uint32_t hash[8], const uint8_t data[], uint32_t data_len) {
  uint32_t a, b, c, d, e, f, g, h, s0, s1, T1, T2;
  uint32_t X[16], i;

  size_t blocks = data_len / 64;
  while (blocks--) {
    a = hash[0];
    b = hash[1];
    c = hash[2];
    d = hash[3];
    e = hash[4];
    f = hash[5];
    g = hash[6];
    h = hash[7];

    for (i = 0; i < 16; i++) {
      X[i] = B2U32(data[0], 24) | B2U32(data[1], 16) | B2U32(data[2], 8) |
             B2U32(data[3], 0);
      data += 4;

      T1 = h;
      T1 += Sigma1(e);
      T1 += Ch(e, f, g);
      T1 += K256[i];
      T1 += X[i];

      T2 = Sigma0(a);
      T2 += Maj(a, b, c);

      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;
    }

    for (; i < 64; i++) {
      s0 = X[(i + 1) & 0x0f];
      s0 = sigma0(s0);
      s1 = X[(i + 14) & 0x0f];
      s1 = sigma1(s1);

      T1 = X[i & 0xf] += s0 + s1 + X[(i + 9) & 0xf];
      T1 += h + Sigma1(e) + Ch(e, f, g) + K256[i];
      T2 = Sigma0(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;
    }

    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;
  }
}

#include <stdio.h>
#include <string.h>
void sha256(uint8_t message[], uint8_t hash[32], size_t message_len) {
  /* initial hash */
  ((uint32_t*)hash)[0] = 0x6a09e667;
  ((uint32_t*)hash)[1] = 0xbb67ae85;
  ((uint32_t*)hash)[2] = 0x3c6ef372;
  ((uint32_t*)hash)[3] = 0xa54ff53a;
  ((uint32_t*)hash)[4] = 0x510e527f;
  ((uint32_t*)hash)[5] = 0x9b05688c;
  ((uint32_t*)hash)[6] = 0x1f83d9ab;
  ((uint32_t*)hash)[7] = 0x5be0cd19;

  sha256_process((uint32_t*)hash, message, message_len);
  for (size_t i = 0; i < 8; i++) {
    uint8_t aux1 = hash[(4 * i) + 0];
    uint8_t aux2 = hash[(4 * i) + 1];
    hash[(4 * i) + 0] = hash[(4 * i) + 3];
    hash[(4 * i) + 1] = hash[(4 * i) + 2];
    hash[(4 * i) + 2] = aux2;
    hash[(4 * i) + 3] = aux1;
  }
}