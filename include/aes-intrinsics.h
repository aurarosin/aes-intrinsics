#include <wmmintrin.h>

static inline __m128i AES_128_ASSIST(__m128i temp1, __m128i temp2);
void AES_128_Key_Expansion(const unsigned char *key,
                           unsigned char *key_expanded);

void AES_ECB_encrypt(const unsigned char *in, unsigned char *out,
                     unsigned long length, const char *key,
                     int number_of_rounds);
void AES_ECB_decrypt(const unsigned char *in, unsigned char *out,
                     unsigned long length, const char *key,
                     int number_of_rounds);

void AES_CBC_encrypt(const unsigned char *in, unsigned char *out,
                     unsigned char ivec[16], unsigned long length,
                     unsigned char *key, int number_of_rounds);
void AES_CBC_decrypt(const unsigned char *in, unsigned char *out,
                     unsigned char ivec[16], unsigned long length,
                     unsigned char *key, int number_of_rounds);

void AES_CTR_encrypt(const unsigned char *in, unsigned char *out,
                     const unsigned char ivec[8], const unsigned char nonce[4],
                     unsigned long length, const unsigned char *key,
                     int number_of_rounds);