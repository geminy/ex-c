#include <stdio.h>

#define LOG(...) \
	printf(__VA_ARGS__); \
	printf("\n");

#define INIT __attribute__((constructor))
#define FINAL __attribute__((destructor))

void INIT initialize()
{
	LOG(__func__)
}

void FINAL finalize()
{
	LOG(__func__)
}

void INIT initialize1()
{
	LOG(__func__)
}

void FINAL finalize1()
{
	LOG(__func__)
}

int main()  
{
	LOG(__func__)

	return 0;
}

void INIT initialize2()
{
	LOG(__func__)
}

void FINAL finalize2()
{
	LOG(__func__)
}

void INIT initialize3()
{
	LOG(__func__)
}

void FINAL finalize3()
{
	LOG(__func__)
}
