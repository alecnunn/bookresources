/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Pseudorandom number generator test harness
// (C) 2000 Guy W. Lecky-Thompson

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prandgen.h"

pseudorandom * rand_gen;

int main (int argc, char * argv[])
{
	long lSeed, lMax, lRand, i;
	int nPad, nPerLine, nRuns, nLinePos;

	// Expect the seed, maximum and runs as arguments
	if (argc < 4)
	{
		printf("\nPrandgen <seed> <n_max> <n_runs>\n");
		return -1;
	}

	lSeed = atol(argv[1]);
	lMax = atol(argv[2]);
	nPad = strlen(argv[2]); // The width of field
	nRuns = atoi(argv[3]);

	rand_gen = new pseudorandom(); // Create a new random number object
	rand_gen->SRand(lSeed);	       // Initialise it with the seed value

	nPerLine = (79-nPad) / nPad;   // Calculate number of numbers per line

	nLinePos = 0;
	while (nRuns > 0)
	{
		if (nLinePos > nPerLine)
		{
			nLinePos = 0;
			printf("\n");
		}

		lRand = rand_gen->Rand(lMax);
		printf("%ld",lRand);
		if (lRand == 0) lRand = 1;
		for (i = lRand; i < lMax; i = (i * 10)) printf(" ");

		nRuns--;
		nLinePos++;
	}

	delete rand_gen;  // Get rid of the object tidily
}

