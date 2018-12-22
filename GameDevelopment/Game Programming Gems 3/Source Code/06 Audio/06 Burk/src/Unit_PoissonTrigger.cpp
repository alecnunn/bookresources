#include "ggsynth.h"
#include "Unit_PoissonTrigger.h"

/**
 *	Unit_PoissonTrigger generates randomly sized triggers with a Poisson distribution.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_PoissonTrigger::Unit_PoissonTrigger()
{
    randSeed = 22222;
}
	
Unit_PoissonTrigger::~Unit_PoissonTrigger()
{
}

void Unit_PoissonTrigger::gen( GGSynthFloat amplitude, GGSynthFloat rate )
{
    int i;
    // Higher rate leads to lower threshold.
    // random() is more likely for to be above a lower threshold
    // so we get more impulses.
    GGSynthFloat threshold = 1.0f - rate;

    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = (randomFloat() > threshold) ? 
            (amplitude * randomFloat()) : 0.0f ;
    }
}
