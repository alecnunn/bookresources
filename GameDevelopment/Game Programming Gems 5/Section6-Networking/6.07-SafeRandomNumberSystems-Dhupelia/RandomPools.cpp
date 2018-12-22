// Includes
#include <iostream>
#include <assert.h>
#include "Pools.h"

// Define a seed value to start the application
#define STARTING_SEED_VALUE 123456

// Program entry-point
int main(int argc, char* argv[])
{
	// Local variable
	unsigned int pool = 0;

	// Seed the random pools
	//  (the standard C call is overriden by the Pools macro)
	srand(STARTING_SEED_VALUE);

	// Get a pointer to the Pools singleton class
	Pools* randptr = Pools::getInstance();

	// Print out the seed values of each random pool
	for (pool = 0; pool < POOL_MAX_TYPES; pool++)
	{
		printf("Pool [%d] = [%d]\n",
				pool,
				randptr->getPoolSeed((POOL_TYPE)pool));
	}

	// Print a blank line for readability
	printf("\n");

	// Get some random numbers
	unsigned int x = 0;
	x = randptr->getRandomNumber(POOL_AI);
	x = randptr->getRandomNumber(POOL_COMMENTARY);
	x = randptr->getRandomNumber(POOL_MUSIC);
	x = randptr->getRandomNumber(POOL_DEFAULT);
	x = randptr->getRandomNumber(POOL_MUSIC);

	// Get more random numbers which will utilize the Pools macro
	x = rand();
	x = rand();

	// Print out the seed values again for each random pool
	for (pool = 0; pool < POOL_MAX_TYPES; pool++)
	{
		printf("Pool [%d] = [%d]\n",
				pool,
				randptr->getPoolSeed((POOL_TYPE)pool));
	}

	// Print a blank line for readability
	printf("\n");

	// Wait for user to press enter before ending console program
	int nextKey = 0;
	printf("Press Enter to Continue\n");
	nextKey = getchar();

	return 0;
}
