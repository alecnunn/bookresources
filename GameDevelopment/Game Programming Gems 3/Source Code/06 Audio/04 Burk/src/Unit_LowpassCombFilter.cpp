#include <stdlib.h>
#include <math.h>
#include "ggsynth.h"
#include "Unit_LowpassCombFilter.h"

/**
 *	Unit_LowpassCombFilter delays a signal by a fixed amount. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_LowpassCombFilter::Unit_LowpassCombFilter()
{
}

Unit_LowpassCombFilter::Unit_LowpassCombFilter( double seconds )
{
    init(seconds);
    setGain( 0.7f, 0.5f );
    zn1 = 0.0f;
}
	
Unit_LowpassCombFilter::~Unit_LowpassCombFilter()
{
}


/***********************************************************************/
void Unit_LowpassCombFilter::setGain( GGSynthFloat pgain, GGSynthFloat highCut )
{
    Unit_CombFilter::setGain( pgain );
    gain1 = highCut;
    gain2 = pgain * ( 1.0f - highCut);
}

/***********************************************************************/
void Unit_LowpassCombFilter::gen( GGSynthFloat *inputs )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        // read from delay line
        GGSynthFloat y = delayLine[cursor];

        outputs[i] = y;

        // write to delay line
        delayLine[cursor] = inputs[i] + (gain2 * zn1);

        // IIR lowpass filter in feedback loop
        zn1 = y + (zn1 * gain1);

        // increment and wrap cursor
        cursor += 1;
        if( cursor >= numFrames ) cursor = 0;
    }
    
// Apply a small impulse to prevent arithmetic underflow,
// which can cause the FPU to interrupt the CPU.
	delayLine[cursor] += (GGSynthFloat) 1.0E-26;

}
