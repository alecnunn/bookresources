#include <math.h>
#include "ggsynth.h"
#include "Unit_Integrator.h"

/**
 *	Unit_Integrator integrates its input and clips it within limits.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_Integrator::Unit_Integrator()
{
    value = 0.0f;
    minimum = 0.0f;
    maximum = 1.0f;
}
	
Unit_Integrator::~Unit_Integrator()
{
}

GGSynthFloat Unit_Integrator::calculateIncrement( GGSynthFloat seconds )
{
    GGSynthFloat delta = maximum - minimum;
    return delta / (seconds * UnitGen::sampleRate );
}

/* Only calculate one value for control rate synthesis. */
void Unit_Integrator::gen1( GGSynthFloat input )
{
    value += (input * GGSYNTH_FRAMES_PER_BLOCK); // integrate

    if( value > maximum ) value = maximum; // clip
    else if( value < minimum ) value = minimum;

    outputs[0] = value;
}

void Unit_Integrator::gen( GGSynthFloat input )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        value += input; // integrate

        if( value > maximum ) value = maximum; // clip
        else if( value < minimum ) value = minimum;

        outputs[i] = value;
    }
}

void Unit_Integrator::gen( GGSynthFloat *inputs )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        value += inputs[i]; // integrate

        if( value > maximum ) value = maximum; // clip
        else if( value < minimum ) value = minimum;

        outputs[i] = value;
    }
}
