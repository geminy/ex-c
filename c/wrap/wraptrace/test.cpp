#include <iostream>
#include <stdlib.h>

#ifdef DEBUGLOG
#define LOG(...) \
	std::cout << "----------" << __VA_ARGS__ << "----------" << std::endl;
#else
#define LOG(...) \
	(void*)0;
#endif // DEBUGLOG

void Test5()
{
	LOG("malloc")
	char *c = (char*)malloc(sizeof(char));
	LOG("malloc")
	
	LOG("calloc")
	char *c2 = (char*)calloc(1, sizeof(char));
	LOG("calloc")

	LOG("realloc")
	c2 = (char*)realloc(c2, sizeof(char) * 2);
	LOG("realloc")

	LOG("free")
	free(c);
	LOG("free")

	LOG("new")
	int *t = new int(1);
	LOG("new")
	
	LOG("new[]")
	int *t2 = new int[2];
	LOG("new[]")

	LOG("delete")
	delete t;
	LOG("delete")
	
	LOG("delete[]")
	delete []t2;
	LOG("delete[]")
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
