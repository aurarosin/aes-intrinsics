#include <stdlib.h>
#define RESERVED 0x80
#define ADATA_ON 0x40

unsigned char* formatting_input_data(unsigned char* N, size_t n,
                                     unsigned char* A, size_t a,
                                     unsigned char* P, size_t p, size_t t,
                                     size_t* r);