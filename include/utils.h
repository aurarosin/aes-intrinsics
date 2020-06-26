unsigned char* hex_to_bytes(const char* hex_str);
char* bytes_to_hex(unsigned char* bytes, int len);
unsigned char* bit_padding(unsigned char* in, int len);
unsigned char* cms_padding(unsigned char* in, int len);
unsigned char* without_cms_padding(unsigned char* in, int len);
