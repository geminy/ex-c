#include <stdlib.h>
#include "hook3.h"

int main()
{
	help();
	char *c = (char*)malloc(sizeof(char));
	free(c);

	return 0;
} 
