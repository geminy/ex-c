#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LOG(flag) \
	printf("%s [%s]\n", __func__, #flag);

void __attribute__((constructor)) init()
{
	LOG(constructor)
}

void __attribute__((destructor)) final()
{
	LOG(destructor)
}

void for_exit()
{
	LOG(atexit)
}

void for_exit2()
{
	LOG(atexit)
}

void for_onexit(int i, void *p)
{
	LOG(on_exit)
}

int main(int argc, char* argv[])
{
	LOG(begin)

	int e = atexit(for_exit);
	printf("atexit(for_exit) %d\n", e);
	e = atexit(for_exit2);
	printf("atexit(for_exit2) %d\n", e);

	//e = on_exit(for_onexit, 0);
	//printf("on_exit(for_onexit) %d\n", e);

	LOG(end)

#ifdef EXIT_NORMAL
	LOG(exit(0))
	exit(0);
#else
# ifdef EXIT_UNNORMAL_L
	LOG(_exit(0))
	_exit(0);
# elif defined EXIT_UNNORMAL_U
	LOG(_Exit(0))
	_Exit(0);
# else
	LOG(return 0)
	return 0;
# endif
#endif 
}
