#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUGLOG
#define LOG(...) \
	printf("%s\n", __VA_ARGS__);
#else
#define LOG(...) \
	(void*)0;
#endif // DEBUGLOG

void Test5()
{
	LOG("malloc")
	char *c = (char*)malloc(sizeof(char));
	LOG("malloc")

	LOG("free")
	free(c);
	LOG("free")
}

void Test4()
{
	Test5();
}

void Test3()
{
	Test4();
}

void Test2()
{
	Test3();
}

void Test1()
{
	Test2();
}

void Test()
{
	Test1();
}

int main()  
{
	Test();

	return 0;
}
