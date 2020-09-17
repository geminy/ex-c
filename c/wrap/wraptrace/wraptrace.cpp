#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <execinfo.h>
#include <cxxabi.h>

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

extern "C" {

// for demangle
size_t FuncNameSize = 256;
char *FuncName = (char*)malloc(FuncNameSize); // not freed

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

	// using demangle
	trace = backtrace_symbols(buffer, size);
	if (NULL == trace) {
		return;
	}
	for (int i = 0; i < size; ++i) {
        char *begin_name = 0;
		char *begin_offset = 0;
		char *end_offset = 0;
        for (char *p = trace[i]; *p; ++p) {
            if (*p == '(') {
                begin_name = p;
			}			
            else if (*p == '+' && begin_name) {
                begin_offset = p;
			}
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
           }
        }
        if (begin_name && begin_offset && end_offset ) {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';
            int status = -4; // 0 -1 -2 -3
            char *ret = abi::__cxa_demangle(begin_name, FuncName, &FuncNameSize, &status);
            if (0 == status) {
                FuncName = ret;
                printf("%s:%s+%s\n", trace[i], FuncName, begin_offset);
            }
            else {
                printf("%s:%s()+%s\n", trace[i], begin_name, begin_offset);
            }
		}
        else {
        	printf("%s\n", trace[i]);
        }
	}
	printf("----------done----------\n\n");
	//free(trace); // free needed, but recursive here 
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

#ifdef WRAP_CALLOC
void* __real_calloc(size_t nmemb, size_t size);
void* __wrap_calloc(size_t nmemb, size_t size)
{
	DumpTrace();
	DLOG(__func__)
	DLOG("\n");
	void *ret = __real_calloc(nmemb, size);
    return ret;
}
#endif // WRAP_CALLOC

#ifdef WRAP_REALLOC
void* __real_realloc(void *ptr, size_t size);
void* __wrap_realloc(void *ptr, size_t size)
{
	DumpTrace();
	DLOG(__func__)
	DLOG("\n");
	void *ret = __real_realloc(ptr, size);
    return ret;
}
#endif // WRAP_REALLOC

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

} // extern "C"

#ifdef WRAP_NEW_1
void* operator new(size_t size)
{
	DLOG(__func__)
	DLOG("\n");
    void *ret = malloc(size);
    return ret;
}
#endif // WRAP_NEW_1

#ifdef WRAP_DELETE_1
void operator delete(void *ptr)
{
	DLOG(__func__)
	DLOG("\n");
    free(ptr);
}
#endif // WRAP_DELETE_1
