#include <stdio.h>
#include <stdlib.h>
#include "attr2.h"

#define DLOG(format, args...) \
	printf(format, ##args);

void init()
{
	DLOG(__func__)
	DLOG("\n")
}

void final()
{
	DLOG(__func__)
	DLOG("\n")
}

void doing()
{
	DLOG(__func__)
	DLOG("\n")
}
