#include <stdio.h>
#include <stdlib.h>

int main()
{
	void *v = malloc(20);
	if (v) {
		printf("v:%p\n", v);
		char *c = (char*)v;
		printf("c:%p c+1:%p\n", c, c + 1);
		int *i = (int*)v;
		printf("i:%p i+1:%p\n", i, i + 1);
		free(v);
		v = NULL;
	}
	return 0;
}
