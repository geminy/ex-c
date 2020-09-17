#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

#define LOG(flag) \
	printf("%s [%s]\n", __func__, #flag);

void dump_trace()
{
	void *buffer[100] = { NULL };
	char **trace = NULL;

	int size = backtrace(buffer, 10);
	trace = backtrace_symbols(buffer, size);
	
	if (NULL != trace) {
		int i = 0;
		for (i = 0; i < size; ++i) {
			printf("%s\n", trace[i]);
		}
		free(trace);
	}
}

void signal_handler(int sig)
{
	printf("signal %d\n", sig);
	dump_trace();
	abort();
}

void test_signal()
{
	LOG(begin)
	char *c = NULL;
	*c = 'a';
	LOG(end)
}

void __attribute__((constructor)) init()
{
	LOG(constructor)
	signal(SIGSEGV, signal_handler);
}

void __attribute__((destructor)) final()
{
	LOG(destructor)
}

int main(int argc, char* argv[])
{
	LOG(begin)

	test_signal();

	LOG(end)

	return 0;
}
