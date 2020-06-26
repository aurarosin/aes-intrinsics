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
  // unsigned char ivec[16] = {34, 15,  20, 79,  33,  7, 1,  99,
  //                           58, 109, 12, 218, 172, 4, 86, 42};
  unsigned char ivec[16];
  memset(ivec, 0, sizeof(unsigned char) * 16);

  AES_128_Key_Expansion((const unsigned char*)key_bytes, key_expanded);
  // printf("key_expanded : %s\n",bytes_to_hex(key_expanded,16 * (ROUNDS + 1)));
  AES_CBC_encrypt(text_bytes, encrypt_bytes, ivec, newLen,
                  (const char*)key_expanded, ROUNDS);

  encrypt = bytes_to_hex(encrypt_bytes, newLen);

  return encrypt;
}

char* text_decrypt(char* encrypt, char* key_bytes) {
  int len = strlen(encrypt);
  unsigned char* encrypt_bytes = hex_to_bytes(encrypt);
  char* decrypt;

  unsigned char key_expanded[16 * (ROUNDS + 1)];
  unsigned char decrypt_bytes[len / 2];
  // unsigned char ivec[16] = {34, 15,  20, 79,  33,  7, 1,  99,
  //                          58, 109, 12, 218, 172, 4, 86, 42};
  unsigned char ivec[16];
  memset(ivec, 0, sizeof(unsigned char) * 16);

  AES_128_Key_Expansion((const unsigned char*)key_bytes, key_expanded);
  printf("encrypt_bytes: %s\n", bytes_to_hex(encrypt_bytes, len / 2));
  printf("key_expanded: %s\n", bytes_to_hex(key_expanded, 16 * (ROUNDS + 1)));

  AES_CBC_decrypt(encrypt_bytes, decrypt_bytes, ivec, len / 2,
                  (const char*)key_expanded, ROUNDS);

  decrypt = bytes_to_hex(decrypt_bytes, len / 2);

  return decrypt;
}
