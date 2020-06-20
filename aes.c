#include "aes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes-intrinsics.h"

unsigned char* hex_to_bytes(char* hex) {}

char* bytes_to_hex(unsigned char* bytes) {
  char* hex = (char*)malloc(sizeof(char) * 16);

  for (int i = 0; i < 16; i++) {
    sprintf(&hex[i * 2], "%02x", bytes[i]);
  }

  return hex;
}

unsigned char* padding(unsigned char* in, int len) {
  int restante = len % 16;
  unsigned char* result =
      (unsigned char*)malloc(sizeof(unsigned char) * (len + restante));

  memcpy(result, in, len);

  if (restante > 0) {
    result[len] = 0x80;
  }

  for (int i = 1; i < restante; i++) {
    result[len + i] = 0;
  }

  return result;
}

char* text_encrypt(char* text, char* key_bytes) {
  int len = strlen(text);
  unsigned char* text_bytes = padding((unsigned char*)text, len);
  // const unsigned char key_bytes[16] = key;

  unsigned char key_expanded[16 * 11];
  unsigned char encrypt_bytes[16];
  const unsigned char dencrypt_bytes[16];

  AES_128_Key_Expansion(key_bytes, key_expanded);
  AES_ECB_encrypt(text_bytes, encrypt_bytes, 16, key_expanded, ROUNDS);
}
