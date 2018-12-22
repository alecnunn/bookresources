/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Serialized usage of the Prand generator
// (C) 2000 Guy W. Lecky-Thompson

// Description :
//  Using a given point in space x,y and a seed n, we should be able to
//  seed the generator on that using a formula, so that the presence of
//  an object at x,y can be calculated on one call, and not by having to
//  regenerate the entire object space.

#include <stdlib.h>
#include <stdio.h>

#include "prandgen.h"

pseudorandom * rand_gen;

int main (int argc, char * argv[])
{
	int nProbability;
	int x,y;
	long lSeed;

	// On the cmd line : probability of an object, seed
	if (argc < 3)
	{
		printf("\nSerial <object_probability %%> <seed>\n");
      		return -1;
	}

	nProbability = atoi(argv[1]);
	lSeed = atol(argv[2]);

	// Create the random number object & seed it
	rand_gen = new pseudorandom();
	rand_gen->SRand(lSeed);

	for (y = 0; y < 20; y++)
	{
		for (x = 0; x < 70; x++)
		{
			// For a given point in space x,y seed on it
			rand_gen->SRand(lSeed + ((x * y) + x));
			if (rand_gen->Rand(100) <= nProbability)
			{
				printf("*");
			}
			else
			{
				printf(" ");
			}
		}
		printf("\n");
	}
}

