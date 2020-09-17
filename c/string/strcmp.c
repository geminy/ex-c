int strcmp (const char *p1, const char *p2)
{
    const unsigned char *s1 = (const unsigned char*)p1;
    const unsigned char *s2 = (const unsigned char*)p2;
    unsigned char c1, c2;

    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0')//如果c1 == '\0', 字符串末尾, 比较结束.
            return c1 - c2;
    }
    while (c1 == c2);//如果c2 == '\0' && c1 != '\0'，比较也会结束.

    return c1 - c2;
}
