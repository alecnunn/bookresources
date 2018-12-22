#include <stdlib.h>
#include <math.h>
#include "ggsynth.h"
#include "Unit_InterpolatingDelay.h"

/**
 *	Unit_InterpolatingDelay delays a signal by a fixed amount. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_InterpolatingDelay::Unit_InterpolatingDelay()
{
}

Unit_InterpolatingDelay::Unit_InterpolatingDelay( double seconds )
{
    init( seconds );
}
	
Unit_InterpolatingDelay::~Unit_InterpolatingDelay()
{
}

/***********************************************************************/
void Unit_InterpolatingDelay::gen( GGSynthFloat *inputs, GGSynthFloat *delays )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        // write to delay line
        delayLine[cursor] = inputs[i];
        
        // calculate how far back to go
        GGSynthFloat floatDelta = delays[i] * UnitGen::sampleRate;
        int delta = (int) floatDelta;
        if( delta > numFrames ) delta = numFrames;

        // read from delay line
        int lookup = cursor - delta;
        if( lookup < 0 ) lookup += numFrames;

        GGSynthFloat target = delayLine[lookup];

        // read from delay line
        lookup += 1;
        if( lookup >= numFrames ) lookup = 0;

        GGSynthFloat source = delayLine[lookup];

        // interpolating between samples
        GGSynthFloat phase = floatDelta - delta;

        outputs[i] = source + phase * ( target - source );

        // increment and wrap cursor
        cursor += 1;
        if( cursor >= numFrames ) cursor = 0;

    }
}
