#define ROUNDS 10
#define IV \
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  //{ 34, 15, 20, 79, 33, 7, 1, 99, 58, 109, 12, 218, 172, 4, 86, 42 }

unsigned char* hex_to_bytes(const char* hex_str);
char* bytes_to_hex(unsigned char* bytes, int len);
unsigned char* padding(unsigned char* in, int len);
char* text_encrypt(char* text, char* key_bytes);
char* text_decrypt(char* encrypt, char* key_bytes);
