#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define LOGF(fd, format, ...) \
	fprintf(fd, format, __VA_ARGS__);

#define LOG(format, ...) \
	LOGF(stdout, format, __VA_ARGS__)

#define HOOK_INIT __attribute__((constructor))

static void* (*old_malloc_hook)(size_t, const void*);

static void* my_malloc(size_t size, const void *caller)
{
	LOG("%s\n", __func__)
	__malloc_hook = old_malloc_hook;
	void *ptr = malloc(size);
	__malloc_hook = my_malloc;
	LOG("malloc size: %lu addr: %p\n", size, ptr)
	return ptr;
}

void HOOK_INIT hook_init()
{
	old_malloc_hook = __malloc_hook;
	__malloc_hook = my_malloc;
}

int main()
{
	char *c = (char*)malloc(sizeof(char));
	free(c);

	return 0;
} 
