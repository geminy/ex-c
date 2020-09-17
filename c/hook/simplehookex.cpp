#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

static void* (*old_malloc_hook)(size_t, const void*);

static void* my_malloc(size_t size, const void *caller)
{
	static int malloc_time = 0;
	__malloc_hook = old_malloc_hook;
	void *ptr = malloc(size);
	__malloc_hook = my_malloc;
	printf("%s, addr: %p, size: %lu, time:%d\n", __func__, ptr, size, ++malloc_time);
	return ptr;
}

void __attribute__((constructor)) malloc_init()
{
	old_malloc_hook = __malloc_hook;
	__malloc_hook = my_malloc;
}

int main()
{
	char *c = (char*)malloc(sizeof(char));
	free(c);

	int *i = new int;
	delete i;

	FILE *f = fopen("./file", "r");
	if (NULL != f) {
		fclose(f);
	}

	return 0;
} 
