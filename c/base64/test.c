#include <stdio.h>
#include <string.h>
#include "base64.h"

int main(int argc, char**argv)
{
    const char *str = "A";
    char *base64 = encode_base64(str, strlen(str));
    printf("[%lu]%s > [%lu]%s\n", strlen(str), str, strlen(base64), base64);

    const char *str2 = "AB";
    char *base642 = encode_base64(str2, strlen(str2));
    printf("[%lu]%s > [%lu]%s\n", strlen(str2), str2, strlen(base642), base642);

    const char *str3 = "ABC";
    char *base643 = encode_base64(str3, strlen(str3));
    printf("[%lu]%s > [%lu]%s\n", strlen(str3), str3, strlen(base643), base643);

    const char *str4 = "ABCA";
    char *base644 = encode_base64(str4, strlen(str4));
    printf("[%lu]%s > [%lu]%s\n", strlen(str4), str4, strlen(base644), base644);

    const char *str5 = "ABCAB";
    char *base645 = encode_base64(str5, strlen(str5));
    printf("[%lu]%s > [%lu]%s\n", strlen(str5), str5, strlen(base645), base645);

    const char *str6 = "ABCABC";
    char *base646 = encode_base64(str6, strlen(str6));
    printf("[%lu]%s > [%lu]%s\n", strlen(str6), str6, strlen(base646), base646);

    printf("------------------\n");

    unsigned int len = 0;
    char *decode = decode_base64(base64, &len);
    printf("[%lu]%s > [%d]%s\n", strlen(base64), base64, len, decode);

    unsigned int len2 = 0;
    char *decode2 = decode_base64(base642, &len2);
    printf("[%lu]%s > [%d]%s\n", strlen(base642), base642, len2, decode2);

    unsigned int len3 = 0;
    char *decode3 = decode_base64(base643, &len3);
    printf("[%lu]%s > [%d]%s\n", strlen(base643), base643, len3, decode3);

    unsigned int len4 = 0;
    char *decode4 = decode_base64(base644, &len4);
    printf("[%lu]%s > [%d]%s\n", strlen(base644), base644, len4, decode4);

    unsigned int len5 = 0;
    char *decode5 = decode_base64(base645, &len5);
    printf("[%lu]%s > [%d]%s\n", strlen(base645), base645, len5, decode5);

    unsigned int len6 = 0;
    char *decode6 = decode_base64(base646, &len6);
    printf("[%lu]%s > [%d]%s\n", strlen(base646), base646, len6, decode6);

    return 0;
}
