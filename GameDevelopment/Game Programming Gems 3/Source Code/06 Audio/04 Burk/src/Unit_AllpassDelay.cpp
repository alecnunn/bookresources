#include <stdlib.h>
#include <math.h>
#include "ggsynth.h"
#include "Unit_AllpassDelay.h"

/**
 *	Unit_AllpassDelay delays a signal by a fixed amount. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_AllpassDelay::Unit_AllpassDelay()
{
}
Unit_AllpassDelay::Unit_AllpassDelay( double seconds )
{
    init( seconds );
    setGain( 0.7f );
}
	
Unit_AllpassDelay::~Unit_AllpassDelay()
{
}

/***********************************************************************/
void Unit_AllpassDelay::setGain( GGSynthFloat pgain )
{
    Unit_CombFilter::setGain( pgain );
    omg2 = 1.0f - (pgain * pgain);
}

/***********************************************************************/
void Unit_AllpassDelay::gen( GGSynthFloat *inputs )
{
    for( int i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        // read from delay line
        GGSynthFloat zmd = delayLine[cursor];

        outputs[i] = (omg2 * zmd) - (inputs[i] * gain);

        // write to delay line
        delayLine[cursor] = inputs[i] + (gain * zmd);

        // increment and wrap cursor
        cursor += 1;
        if( cursor >= numFrames ) cursor = 0;
    }
    
// Apply a small impulse to prevent arithmetic underflow,
// which can cause the FPU to interrupt the CPU.
	delayLine[cursor] += (GGSynthFloat) 1.0E-26;

}
