#include "aes.h"

int main() {
  char* text = "Hola";
  char* key = "1234567890123456";

  text_encrypt(text, key);
  // Encrypt: 92E9BB5C83C5DAF00CE9136544A90AA5

  return 0;
}