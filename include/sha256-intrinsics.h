#include <stddef.h>
#include <stdint.h>

void sha256_intrinsics(uint8_t *message, uint8_t hash[32], size_t message_len);
