#include "aes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes-intrinsics.h"
#include "utils.h"

char* text_encrypt(char* text, char* key_bytes) {
  int len = strlen(text);
  unsigned char* text_bytes = bit_padding((unsigned char*)text, len);
  int newLen = (len % 16 == 0) ? len : len + (16 - len % 16);

  char* encrypt;

  unsigned char key_expanded[16 * (ROUNDS + 1)];
  unsigned char encrypt_bytes[newLen];

  AES_128_Key_Expansion((const unsigned char*)key_bytes, key_expanded);
  AES_ECB_encrypt(text_bytes, encrypt_bytes, newLen, (const char*)key_expanded,
                  ROUNDS);

  encrypt = bytes_to_hex(encrypt_bytes, newLen);

  return encrypt;
}

char* text_decrypt(char* encrypt, char* key_bytes) {
  int len = strlen(encrypt);
  unsigned char* encrypt_bytes = hex_to_bytes(encrypt);
  char* decrypt;

  unsigned char key_expanded[16 * 11];
  unsigned char decrypt_bytes[len / 2];

  AES_128_Key_Expansion((const unsigned char*)key_bytes, key_expanded);
  AES_ECB_decrypt(encrypt_bytes, decrypt_bytes, len / 2,
                  (const char*)key_expanded, ROUNDS);

  decrypt = bytes_to_hex(decrypt_bytes, len / 2);

  return decrypt;
}
