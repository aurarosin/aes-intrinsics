#include <stddef.h>

void gcm_encrypt(unsigned char* nonce, size_t n, unsigned char* A, size_t a,
                 unsigned char* P, size_t p, unsigned char* key, size_t Tlen,
                 unsigned char* out);
int gcm_decrypt(unsigned char* nonce, size_t n, unsigned char* A, size_t a,
                unsigned char* C, size_t c, unsigned char* key, size_t Tlen,
                unsigned char* out_P);
