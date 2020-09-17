#include <stdio.h>
#include <string.h>

int main()
{
    char arr[20] = "apple";
    const char *p = "pear";
    char *p2 = strcat(arr, p);
    printf("arr=%s,p=%s,p2=%s\n", arr, p, p2);//arr=applepear,p=pear,p2=applepear


    const char *pc = "abc";
    const char *pc2 = "abcdefg";
    const char *pc3 = "abc";
    printf("d:%d\n", 'd');
    printf("strcmp(pc, pc2):%d\n", strcmp(pc, pc2));
    printf("strcmp(pc2, pc):%d\n", strcmp(pc2, pc));
    printf("strcmp(pc, pc3):%d\n", strcmp(pc, pc3));
    //d:100
    //strcmp(pc, pc2):-100
    //strcmp(pc2, pc):100
    //strcmp(pc, pc3):0

    char a[10] = "apple";
    const char *pt = "pear";
    char *pt2= strncat(a, pt, 2);
    printf("a=%s,pt=%s,pt2=%s\n", a, pt, pt2);//a=applepe,pt=pear,pt2=applepe
	
    return 0;
}
