#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* hex_to_bytes(const char* hex_str) {
  int len = strlen(hex_str);
  unsigned char* bytes =
      (unsigned char*)malloc(sizeof(unsigned char) * (len / 2 + 1));

  for (int i = 0; i < len / 2; i++) {
    sscanf(&hex_str[2 * i], "%02x", (unsigned int*)&bytes[i]);
  }

  bytes[len / 2 + 1] = 0;

  return bytes;
}

char* bytes_to_hex(unsigned char* bytes, int len) {
  char* hex = (char*)malloc(sizeof(char) * (2 * len + 1));

  for (int i = 0; i < len; i++) {
    sprintf(&hex[2 * i], "%02x", bytes[i]);
  }

  hex[2 * len + 1] = 0;

  return hex;
}

unsigned char* bit_padding(unsigned char* in, int len) {
  int restante = 16 - (len % 16);
  int newLen = len + restante;

  unsigned char* result =
      (unsigned char*)malloc(sizeof(unsigned char) * newLen);

  memset(result, 0, sizeof(char) * newLen);
  memcpy(result, in, len);

  if (restante > 0) {
    result[len] = 0x80;
  }

  return result;
}