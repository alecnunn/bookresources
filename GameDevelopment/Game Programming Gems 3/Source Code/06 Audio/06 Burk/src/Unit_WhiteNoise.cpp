#include "ggsynth.h"
#include "Unit_WhiteNoise.h"

/**
 *	Unit_WhiteNoises generates noise using linear congruential algorithm. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_WhiteNoise::Unit_WhiteNoise()
{
    randSeed = 22222;
}
	
Unit_WhiteNoise::~Unit_WhiteNoise()
{
}

/************************************************************/
/* Calculate pseudo-random 32 bit number based on linear congruential method. */
unsigned long Unit_WhiteNoise::randomULong( void )
{
	randSeed = NextLinearCongruential(randSeed);
	return randSeed;
}

/************************************************************/
/* Calculate pseudo-random 32 bit number based on linear congruential method. */
float Unit_WhiteNoise::randomFloat( void )
{
	randSeed = NextLinearCongruential(randSeed);
	return ((long)randSeed) * (0.5f/(1<<30));
}

void Unit_WhiteNoise::gen( GGSynthFloat amplitude )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = randomFloat() * amplitude;
    }
}

void Unit_WhiteNoise::gen( GGSynthFloat *amplitudes )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = randomFloat() * amplitudes[i];
    }
}
