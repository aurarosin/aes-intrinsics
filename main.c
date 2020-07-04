#include <stdio.h>
#include <string.h>

#include "sha-intrinsics.h"
#include "utils.h"

void test_sha() {
  char* text = "Hola";
  uint32_t hash[8];
  int nbytes = strlen(text);
  uint8_t* input = cms_padding(text, nbytes);

  sha256(input, hash, 64);

  printf("Text(%d): %s\n", nbytes, text);
  printf("SHA256: %s\n", bytes_to_hex((uint8_t*)hash, 32));
}

int main() {
  test_sha();

  return 0;
}
