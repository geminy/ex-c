#include <stdio.h>

struct S
{
    char a;
    int b;
};

struct N
{
};

struct N2
{
    int p[0];
};

int main()
{
    struct S *s = NULL;
    int size = sizeof *s;

    printf("%lu %d\n", sizeof(struct S), size); // 8 8
    printf("%lu %lu\n", sizeof(struct N), sizeof(struct N2)); // 1 0

    return 0;
}
