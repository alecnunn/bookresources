#include <math.h>
#include "ggsynth.h"
#include "Unit_RedNoise.h"

/**
 *	Unit_RedNoises generates noise using linear congruential algorithm. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_RedNoise::Unit_RedNoise()
{
    phase = 0.0f;
    source = 0.0f;
    target = randomFloat();
    delta = target - source;
}
	
Unit_RedNoise::~Unit_RedNoise()
{
}

#define RED_NOISE_GUTS \
    phase += phaseInc; \
    if( phase >= 1.0 ) \
    { \
        source = target; \
        target = randomFloat(); \
        delta = target - source; \
        phase -= 1.0f; \
    }

/* Only calculate one value for control rate synthesis. */
void Unit_RedNoise::gen1( GGSynthFloat amplitude, GGSynthFloat frequency )
{
    GGSynthFloat phaseInc = (GGSynthFloat) fabs( GGSYNTH_FRAMES_PER_BLOCK * frequency * UnitGen.samplePeriod );

    RED_NOISE_GUTS;
        // interpolate
    outputs[0] = amplitude * (source + phase * delta);
}

void Unit_RedNoise::gen( GGSynthFloat amplitude, GGSynthFloat frequency )
{
    int i;
    GGSynthFloat phaseInc = (GGSynthFloat) fabs( frequency * UnitGen.samplePeriod );

    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        RED_NOISE_GUTS;
        // interpolate
        outputs[i] = amplitude * (source + phase * delta);
    }
}

void Unit_RedNoise::gen( GGSynthFloat *amplitudes, GGSynthFloat *frequencies )
{
    int i;

    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        GGSynthFloat phaseInc = (GGSynthFloat) fabs( frequencies[i] * UnitGen.samplePeriod );
        RED_NOISE_GUTS;
        // interpolate
        outputs[i] = amplitudes[i] * (source + phase * delta);
    }
}

