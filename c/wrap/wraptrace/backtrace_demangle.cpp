#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <execinfo.h>
#include <cxxabi.h>

void my_backtrace()
{
	void *buffer[100] = { NULL };
	char **trace = NULL;

	int size = backtrace(buffer, 100);
	trace = backtrace_symbols(buffer, size);
	if (NULL == trace) {
		return;
	}

	for (int i = 0; i < size; ++i) {
		printf("%s\n", trace[i]);
	}

	free(trace);
	printf("----------done----------\n");
}

void my_backtrace2()
{
	void *buffer[100] = { NULL };
	char **trace = NULL;

	int size = backtrace(buffer, 100);
	trace = backtrace_symbols(buffer, size);
	if (NULL == trace) {
		return;
	}

	size_t name_size = 100;
	char *name = (char*)malloc(name_size);
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
            char *ret = abi::__cxa_demangle(begin_name, name, &name_size, &status);
            if (0 == status) {
                name = ret;
                printf("%s:%s+%s\n", trace[i], name, begin_offset);
            }
            else {
                printf("%s:%s()+%s\n", trace[i], begin_name, begin_offset);
            }
		}
        else {
        	printf("%s\n", trace[i]);
        }
	}
	free(name);
	free(trace);
	printf("----------done----------\n");
}

void func2()
{
	//my_backtrace();
	my_backtrace2();
}

void func()
{
	func2();
}

int main()
{
	func();
	return 0;
}
