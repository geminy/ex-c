#include "test.h"

#include <stdio.h>

int number = 1;
static int number1;
int number2;
extern int number3;

void count()
{
	//static int i;
	static int i = 100;
	printf("count i=%d\n", i++);
}

void count2()
{

}

extern void method();
void method2();

int main()
{
	printf("main cer test\n");

	for (int i = 1; i <= 10; ++i) {
		count();
	}

	printf("main number1=%d\n", number1);

	//int number2 = 100;
	//auto int number2 = 100;
	//extern int number2;
	//number2 = 101;
	printf("main number2=%d\n", number2);
	printf("main number3=%d\n", number3);

	method();
	method2();

	return 0;
}

void test()
{

}

void test2()
{

}

void test3()
{

}

void test4()
{

}