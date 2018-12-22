/*----------------------------------------------------------------------------
 *
 *					HAMMIN CODE EFFICIENCY COMPUTATION
 *					===================================
 *
----------------------------------------------------------------------------*/
#include <stdio.h>
#define MAX_POW	20

main()
{
	int a;
	int _pow = 1;
	int old_pow = 1;
	int N, old_N = 1;

	printf( "* * * hemming code efficiency test * * * by Kris Kaspersky\n"\
			" BLOCK_SIZE    FUEL UP   EFFICIENCY\n"\
			"-----------------------------------\n");
	for (a = 0; a < MAX_POW; a++)
	{
		N = _pow - old_pow - 1 + old_N;

		printf("%8d   %8d   %8.1f%%\n",_pow, N, (float) N/_pow*100);
		
		// NEXT
		old_pow = _pow; _pow = _pow * 2; old_N = N;

	} printf("-----------------------------------\n");
}