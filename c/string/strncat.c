char* strncat(char *s1, const char *s2, size_t n)
{
    char c;
    char *s = s1;

    do
        c = *s1++;
    while (c != '\0');

    s1 -= 2;
    /* 上面的处理同strcat, 把指针移动到合适的位置,
       等待字符串的追加  */

    /* 当n >= 4时, n右移两位, 即除以4, 这样效率更高. */
    if (n >= 4) {
        size_t n4 = n >> 2;
        do {
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
        }
        while (--n4 > 0);
        n &= 3;/* 相当于取n/4的余数, 位操作符的效率比+、-要高，比*、/更高. */
    }

    while (n > 0) {
        c = *s2++;
        *++s1 = c;
        if (c == '\0')
            return s;
        n--;
    }

    if (c != '\0')
        *++s1 = '\0';

    return s;
}
