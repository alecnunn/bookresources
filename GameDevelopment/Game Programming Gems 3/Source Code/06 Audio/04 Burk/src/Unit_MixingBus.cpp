#include <math.h>
#include "ggsynth.h"
#include "Unit_MixingBus.h"

/**
 *	Unit_MixingBus mixes an input signal with the current value.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_MixingBus::Unit_MixingBus()
{
}
	
Unit_MixingBus::~Unit_MixingBus()
{
}

/***********************************************************************/
void Unit_MixingBus::clear( void )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = 0.0f;
    }
}

/***********************************************************************/
void Unit_MixingBus::gen( GGSynthFloat *input )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] += input[i];
    }
}

/***********************************************************************/
void Unit_MixingBus::gen( GGSynthFloat *input, GGSynthFloat amplitude )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] += input[i] * amplitude;
    }
}

void Unit_MixingBus::gen( GGSynthFloat *input, GGSynthFloat *amplitudes )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] += input[i] * amplitudes[i];
    }
}
