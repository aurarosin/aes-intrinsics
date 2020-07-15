#include <math.h>
#include <stdio.h>
#include <string.h>

#include "sha256-intrinsics.h"
#include "sha256.h"
#include "utils.h"

void test_sha() {
  char* text = "Hola";
  size_t text_len = strlen(text);
  uint8_t* message = bit_padding(text, text_len);
  size_t message_len = ceill((text_len + 1) / 64.0) * 64;
  uint8_t hash[32];

  printf("message (%d bytes): %s\n", message_len, bytes_to_hex(message, message_len));
  // sha256_intrinsics(message, hash, message_len);
  sha256(message, hash, message_len);

  printf("Text(%d): %s\n", text_len, text);
  printf("SHA256: %s\n", bytes_to_hex(hash, 32));
}

int main() {
  test_sha();

  return 0;
}
