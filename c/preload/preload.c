#include <stdio.h>
#include <stdlib.h>

void* malloc(size_t size)
{
	printf("%s size: %lu\n", __func__, size);
	return NULL;
}
