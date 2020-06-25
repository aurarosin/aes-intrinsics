#include <immintrin.h>

void sha256(unsigned char* data) {
  __m128i shuf_mask, state0;
  __m128i msg, msgtmp0, state1, state0;
  
  // Rounds 0-3
  msg = _mm_loadu_si128((__m128i*)data);
  msgtmp0 = _mm_shuffle_epi8(msg, shuf_mask);
  msg = _mm_add_epi32(
      msgtmp0, _mm_set_epi64x(0xE9B5DBA5B5C0FBCFull, 0x71374491428A2F98ull));
  state1 = _mm_sha256rnds2_epu32(state1, state0, msg);
  msg = _mm_shuffle_epi32(msg, 0x0E);
  state0 = _mm_sha256rnds2_epu32(state0, state1, msg);

  return
}