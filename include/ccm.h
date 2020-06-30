#include <stddef.h>

#define RESERVED 0x80
#define ADATA_ON 0x40

void ccm(unsigned char* nonce, size_t n, unsigned char* A, size_t a,
         unsigned char* P, size_t p, unsigned char* key, size_t Tlen,
         unsigned char* out);
unsigned char* formatting_input_data(unsigned char* N, size_t n,
                                     unsigned char* A, size_t a,
                                     unsigned char* P, size_t p, size_t t,
                                     size_t* r);
void formatting_ctr(unsigned char* N, size_t n, size_t i, unsigned char* out);
void LSB(unsigned char* x, size_t lenX, size_t s, unsigned char* out);
void MSB(unsigned char* x, size_t lenX, size_t s, unsigned char* out);
