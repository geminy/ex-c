#ifndef BASE64_H
#define BASE64_H

char* encode_base64(const void *src, unsigned int byte);
void* decode_base64(const char *src, unsigned int *byte);

#endif // BASE64_H
