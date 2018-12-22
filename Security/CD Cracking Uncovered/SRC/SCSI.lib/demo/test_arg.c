#include <stdio.h>

main(int argc, char** argv)
{
	int a;
	for (a = 1; a < argc; a++)
	{
		printf("%x - %s\n",a, argv[a]);
	}
}
