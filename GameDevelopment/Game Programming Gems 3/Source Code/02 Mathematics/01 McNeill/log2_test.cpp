#include "log2.h"
#include <stdio.h>

static const unsigned test_value[] =
{
	0, 1, 2, 3, 4,
	255, 256, 257,
	0x7fffffff, 0x80000000, 0x80000001,
	0xfffffffe, 0xffffffff
};
static const int num_test_values = sizeof(test_value) / sizeof(test_value[0]);

void main()
{
	printf( "   n(hex) log2le log2ge mask(hex)\n" );
	for ( int i = 0; i < num_test_values; ++ i )
	{
		unsigned n = test_value[i];
		printf( "%8x: %6d %6d  %8x\n", n, log2le(n), log2ge(n), bitmask(n) );
	}
}
