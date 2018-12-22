// Includes
#include <iostream>
#include <assert.h>
#include "Pools.h"

// Class variable definitions
Pools*			Pools::instance = 0;
unsigned int	Pools::randPools[POOL_MAX_TYPES];
bool			Pools::seeded = false;

// Constructor
Pools::Pools() {}

// Destructor
Pools::~Pools() {}

// Get Instance
Pools* Pools::getInstance()
{
	if (instance == 0)
	{
		instance = new Pools;
	}

	return instance;
}

// Seed the random pools
void Pools::seedPools(unsigned int seedValue)
{
	// Make sure the seed is not 0
	if (seedValue == 0)
	{
		printf("ERROR - TRYING TO SEED THE RANDOM POOLS WITH VALUE OF ZERO\n");
		assert(0);
		return;
	}

	// Make sure we've only seeded the pools once
	if (seeded == true)
	{
		printf("ERROR - TRYING TO SEED THE RANDOM POOLS MORE THAN ONCE\n");
		assert(0);
		return;
	}

	// Loop through the random number pools
	for (unsigned short pool = 0; pool < POOL_MAX_TYPES; pool++)
	{
		// Initialize the random pool to the starting seed value
		randPools[pool] = seedValue;
	}

	// Set our flag to make sure we're never re-initialized by an unknown
	//  spot of code somewhere in the application
	seeded = true;
}

// GET the seed value for a given pool
unsigned int Pools::getPoolSeed(POOL_TYPE whichPool)
{
	return randPools[whichPool];
}

// SET the seed value for a given pool
void Pools::setPoolSeed(POOL_TYPE whichPool, unsigned int newSeed)
{
	// Make sure the seed is not 0
	if (newSeed != 0)
	{
		randPools[whichPool] = newSeed;
	}
}

// This is the actual random number generator
//  http://www.generation5.org/content/2001/ga01.asp
static const unsigned long RDX_RANGE = 0x7FFFFFFF;
static const unsigned long RDX_CONST = 0x00000000000041A7;
static const unsigned long RDX_Q = RDX_RANGE / RDX_CONST;
static const unsigned long RDX_R = RDX_RANGE % RDX_CONST;
unsigned int Pools::getRandomNumber(POOL_TYPE whichPool)
{
	// Return random integer32 between 0 & range value
	unsigned long hi = randPools[whichPool] / RDX_Q;
	unsigned long lo = randPools[whichPool] - (hi * RDX_Q);
	randPools[whichPool] = ((RDX_CONST * lo) - (RDX_R * hi));
	if (randPools[whichPool] == 0) randPools[whichPool] = RDX_RANGE;

	int rslt = int(randPools[whichPool]);
	if (rslt < 0) rslt = -rslt;
	return rslt;

	return 0;
}
