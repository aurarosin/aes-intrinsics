#define ROUNDS 10

unsigned char* hex_to_bytes(char* hex);
char* bytes_to_hex(unsigned char* bytes, int len);
unsigned char* padding(unsigned char* in, int len);
char* text_encrypt(char* text, unsigned char* key);
