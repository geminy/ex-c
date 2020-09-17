#include <stdio.h>
#include <stdlib.h>

#define DLOG(format, args...) \
	printf(format, ##args);

#define BEGIN __attribute__((constructor))
#define END __attribute__((destructor))

void BEGIN init()
{
	DLOG(__func__)
	DLOG("\n")
}

void END final()
{
	DLOG(__func__)
	DLOG("\n")
}

void doing()
{
	DLOG(__func__)
	DLOG("\n")
}

#ifdef WRAP_MALLOC
void* __real_malloc(size_t size);
void* __wrap_malloc(size_t size)
{
	DLOG(__func__)
	DLOG("\n")
	void *ret = __real_malloc(size);
    return ret;
}
#endif // WRAP_MALLOC

