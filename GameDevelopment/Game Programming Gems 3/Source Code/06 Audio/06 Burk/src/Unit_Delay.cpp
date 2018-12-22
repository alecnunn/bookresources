#include <stdlib.h>
#include <math.h>
#include "ggsynth.h"
#include "Unit_Delay.h"

/**
 *	Unit_Delay delays a signal by a fixed amount. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_Delay::Unit_Delay()
{
}
Unit_Delay::Unit_Delay( double seconds )
{
    init(seconds);
}
	
Unit_Delay::~Unit_Delay()
{
    delete[] delayLine;
}

void Unit_Delay::init( double seconds )
{
    numFrames = (int) (seconds * UnitGen::sampleRate);
    if( numFrames < 1 ) numFrames = 1;
    delayLine = new GGSynthFloat[ numFrames ];
    for( int i=0; i<numFrames; i++ )
    {
        delayLine[i] = 0.0f;
    }
    cursor = 0;
}

/***********************************************************************/
void Unit_Delay::gen( GGSynthFloat *inputs )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        // read from delay line
        outputs[i] = delayLine[cursor];

        // write to delay line
        delayLine[cursor] = inputs[i];

        // increment and wrap cursor
        cursor += 1;
        if( cursor >= numFrames ) cursor = 0;
    }
}
