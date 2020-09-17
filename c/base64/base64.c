/*
 * base64 impl
 * encode/decode no considering of 76 chars per line
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define GROUP_STANDARD_BYTE (3)
#define GROUP_BASE64_BYTE (4)

static const char *BASE64CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char PADCHAR = '=';

bool decode_value(char *decodes, const char *src);

char* encode_base64(const void *src, unsigned int byte)
{
    uint32_t nremainder = byte % GROUP_STANDARD_BYTE;
    uint32_t ngroup = (byte / GROUP_STANDARD_BYTE) + (nremainder > 0 ? 1 : 0);
    uint32_t nsize = ngroup * GROUP_BASE64_BYTE;
    char *dst = (char*)malloc(nsize + 1);
    if (!dst) {
        return NULL;
    }
    memset(dst, 0, nsize + 1);

    const char *_src = (const char*)src;
    uint32_t offset = 0;
    while (1) {
        if (1 == ngroup && 0 != nremainder) {
            if (1 == nremainder) {
                dst[offset] = BASE64CHARS[_src[0] >> 2];
                dst[offset + 1] = BASE64CHARS[((_src[0] & 0x03) << 4)];
                dst[offset + 2] = PADCHAR;
                dst[offset + 3] = PADCHAR;
            }
            else {
                dst[offset] = BASE64CHARS[_src[0] >> 2];
                dst[offset + 1] = BASE64CHARS[((_src[0] & 0x03) << 4) | (_src[1] >> 4)];
                dst[offset + 2] = BASE64CHARS[((_src[1] & 0x0f) << 2)];
                dst[offset + 3] = PADCHAR;
            }
        }
        else {
            dst[offset] = BASE64CHARS[_src[0] >> 2];
            dst[offset + 1] = BASE64CHARS[((_src[0] & 0x03) << 4) | (_src[1] >> 4)];
            dst[offset + 2] = BASE64CHARS[((_src[1] & 0x0f) << 2) | (_src[2] >> 6)];
            dst[offset + 3] = BASE64CHARS[_src[2] & 0x3f];
        }

        offset += GROUP_BASE64_BYTE;
        _src += GROUP_STANDARD_BYTE;
        if (--ngroup < 1) {
            break;
        }
    }

    if (offset != nsize) {
        free(dst);
        return NULL;
    }

    return dst;
}

void* decode_base64(const char *src, unsigned int *byte)
{
    uint32_t nlen = strlen(src);
    if (nlen % GROUP_BASE64_BYTE) {
        return NULL;
    }

    uint32_t nsize = nlen / GROUP_BASE64_BYTE * GROUP_STANDARD_BYTE;
    char *dst = (char*)malloc(nsize + 1);
    if (!dst) {
        return NULL;
    }
    memset(dst, 0, nsize + 1);

    char decodes[4] = { 0 };
    for (*byte = 0; *src; src += GROUP_BASE64_BYTE) {
        if (!decode_value(decodes, src)) {
            free(dst);
            return NULL;
        }

        if (PADCHAR == src[2]) {
            if (PADCHAR != src[3]) {
                free(dst);
                return NULL;
            }
            dst[(*byte)++] = (decodes[0] << 2) + ((decodes[1] & 0x30) >> 4);
        }
        else if (PADCHAR == src[3]) {
            dst[(*byte)++] = (decodes[0] << 2) + ((decodes[1] & 0x30) >> 4);
            dst[(*byte)++] = (decodes[1] << 4) + ((decodes[2] & 0x3c) >> 2);
        }
        else {
            dst[(*byte)++] = (decodes[0] << 2) + ((decodes[1] & 0x30) >> 4);
            dst[(*byte)++] = (decodes[1] << 4) + ((decodes[2] & 0x3c) >> 2);
            dst[(*byte)++] = (decodes[2] << 6) + decodes[3];
        }
    }

    return dst;
}

bool decode_value(char *decodes, const char *src)
{
    int i;
    char *p;
    for (i = 0; i < GROUP_BASE64_BYTE; ++i) {
        if (PADCHAR != src[i]) {
            p = strchr(BASE64CHARS, src[i]);
            if (!p) {
                return false;
            }
            decodes[i] = p - BASE64CHARS;
        }
    }

    return true;
}
