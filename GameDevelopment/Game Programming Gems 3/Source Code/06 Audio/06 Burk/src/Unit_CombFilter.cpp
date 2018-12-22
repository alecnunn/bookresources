#include <stdlib.h>
#include <math.h>
#include "ggsynth.h"
#include "Unit_CombFilter.h"

/**
 *	Unit_CombFilter delays a signal by a fixed amount. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_CombFilter::Unit_CombFilter()
{
}

Unit_CombFilter::Unit_CombFilter( double seconds )
{
    init(seconds);
    gain = 0.7f;
}
	
Unit_CombFilter::~Unit_CombFilter()
{
}

/***********************************************************************/
void Unit_CombFilter::setGain( GGSynthFloat pgain )
{
    gain = pgain;
}

/***********************************************************************/
void Unit_CombFilter::gen( GGSynthFloat *inputs )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        // read from delay line
        GGSynthFloat y = delayLine[cursor];

        outputs[i] = y;

        // write to delay line
        delayLine[cursor] = inputs[i] + (gain * y);

        // increment and wrap cursor
        cursor += 1;
        if( cursor >= numFrames ) cursor = 0;
    }
    
// Apply a small impulse to prevent arithmetic underflow,
// which can cause the FPU to interrupt the CPU.
	delayLine[cursor] += (GGSynthFloat) 1.0E-26;

}
