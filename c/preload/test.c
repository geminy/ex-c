#include <stdio.h>
#include <stdlib.h>

int main()
{
	int i = 0;
	for (; i < 5; ++i) {
		char *c = (char*)malloc(sizeof(char));
		if (NULL == c) {
			printf("malloc fails\n");
		}
		else {
			printf("malloc ok\n");
		}
	}
	
	return 0;
}
