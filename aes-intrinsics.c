#include <wmmintrin.h>
//#include <wmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

static inline __m128i AES_128_ASSIST(__m128i temp1, __m128i temp2) {
  __m128i temp3;
  temp2 = _mm_shuffle_epi32(temp2, 0xff);
  temp3 = _mm_slli_si128(temp1, 0x4);
  temp1 = _mm_xor_si128(temp1, temp3);
  temp3 = _mm_slli_si128(temp3, 0x4);
  temp1 = _mm_xor_si128(temp1, temp3);
  temp3 = _mm_slli_si128(temp3, 0x4);
  temp1 = _mm_xor_si128(temp1, temp3);
  temp1 = _mm_xor_si128(temp1, temp2);
  return temp1;
}

void AES_128_Key_Expansion(const unsigned char *userkey, unsigned char *key) {
  __m128i temp1, temp2;
  __m128i *Key_Schedule = (__m128i *)key;
  temp1 = _mm_loadu_si128((__m128i *)userkey);
  Key_Schedule[0] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x1);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[1] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x2);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[2] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x4);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[3] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x8);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[4] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x10);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[5] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x20);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[6] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x40);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[7] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x80);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[8] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x1b);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[9] = temp1;
  temp2 = _mm_aeskeygenassist_si128(temp1, 0x36);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[10] = temp1;
}

/* Note – the length of the output buffer is assumed to be a multiple of 16
 * bytes */
void AES_ECB_encrypt(const unsigned char *in, unsigned char *out,
                     unsigned long length, const char *key,
                     int number_of_rounds) {
  __m128i tmp;
  int i, j;
  // pointer to the PLAINTEXT
  // pointer to the CIPHERTEXT buffer
  // text length in bytes
  // pointer to the expanded key schedule
  // number of AES rounds 10,12 or 14
  if (length % 16)
    length = length / 16 + 1;
  else
    length = length / 16;
  for (i = 0; i < length; i++) {
    tmp = _mm_loadu_si128(&((__m128i *)in)[i]);
    tmp = _mm_xor_si128(tmp, ((__m128i *)key)[0]);
    for (j = 1; j < number_of_rounds; j++) {
      tmp = _mm_aesenc_si128(tmp, ((__m128i *)key)[j]);
    }
    tmp = _mm_aesenclast_si128(tmp, ((__m128i *)key)[j]);
    _mm_storeu_si128(&((__m128i *)out)[i], tmp);
  }
}

void AES_ECB_decrypt(const unsigned char *in, unsigned char *out,
                     unsigned long length, const char *key,
                     int number_of_rounds) {
  __m128i tmp;
  int i, j;
  // pointer to the CIPHERTEXT
  // pointer to the DECRYPTED TEXT buffer
  // text length in bytes
  // pointer to the expanded key schedule
  // number of AES rounds 10,12 or 14
  if (length % 16)
    length = length / 16 + 1;
  else
    length = length / 16;
  for (i = 0; i < length; i++) {
    tmp = _mm_loadu_si128(&((__m128i *)in)[i]);
    tmp = _mm_xor_si128(tmp, ((__m128i *)key)[0]);
    for (j = 1; j < number_of_rounds; j++) {
      tmp = _mm_aesdec_si128(tmp, ((__m128i *)key)[j]);
    }
    tmp = _mm_aesdeclast_si128(tmp, ((__m128i *)key)[j]);
    _mm_storeu_si128(&((__m128i *)out)[i], tmp);
  }
}

int main() {
  int rounds = 10;
  const char* key = "MyKey";
  char* text = "Hello World!";

  const unsigned char text_bytes[200];
  const unsigned char encrypt_bytes[200];
  const unsigned char dencrypt_bytes[200];


  return 0;
}