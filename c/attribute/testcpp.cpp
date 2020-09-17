#include <iostream>

#define LOG(...) \
	std::cout << "----------" << __VA_ARGS__ << "----------" << std::endl;

#define BEGIN_DECLS extern "C" {
#define END_DECLS }

#define INIT __attribute__((constructor))
#define FINAL __attribute__((destructor))

BEGIN_DECLS

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

END_DECLS

int main()  
{
	LOG(__func__)

	return 0;
}
