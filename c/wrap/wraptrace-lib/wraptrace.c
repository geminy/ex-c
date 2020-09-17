#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <execinfo.h>

#ifdef DEBUGLOG
#define DLOG(format, args...) \
	printf(format, ##args);
#else
#define DLOG(format, args...) \
	(void*)0;
#endif // DEBUGLOG

#define TRACE_SIZE (100)
#define DUMP_LOCATION ("./memory_count")
#define MAX_STACK (10)

void DumpTrace(void)
{
	void *buffer[TRACE_SIZE] = { NULL };
	char **trace = NULL;

	int size = backtrace(buffer, TRACE_SIZE);

	int flags = O_CREAT | O_RDWR | O_APPEND;
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	int fd = open(DUMP_LOCATION, flags, mode);
	if (-1 == fd) {
		return;
	}

	int sz = size > MAX_STACK ? MAX_STACK : size;
	backtrace_symbols_fd(buffer, sz, fd);
	// trace = backtrace_symbols(buffer, size); // calls malloc
	write(fd, "--------------------\n\n", 22);

	close(fd);
}

#ifdef WRAP_MALLOC
void* __real_malloc(size_t size);
void* __wrap_malloc(size_t size)
{
	DumpTrace();
	DLOG(__func__)
	DLOG("\n");
	void *ret = __real_malloc(size);
    return ret;
}
#endif // WRAP_MALLOC

#ifdef WRAP_FREE
void __real_free(void *ptr);
void __wrap_free(void *ptr)
{
	DumpTrace();
	DLOG(__func__)
	DLOG("\n");
	__real_free(ptr);
}
#endif // WRAP_FREE
