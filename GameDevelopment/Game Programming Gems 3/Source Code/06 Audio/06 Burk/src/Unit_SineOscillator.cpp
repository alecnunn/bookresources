#include <math.h>
#include "ggsynth.h"
#include "Unit_SineOscillator.h"

/**
 *	Unit_SineOscillators generates a sine wave. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_SineOscillator::Unit_SineOscillator()
{
}
	
Unit_SineOscillator::~Unit_SineOscillator()
{
}

/***********************************************************************/
void Unit_SineOscillator::gen( GGSynthFloat amplitude, GGSynthFloat frequency )
{
    int i;
    GGSynthFloat phaseInc = FREQ_TO_PHASEINC( frequency );
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        INCR_WRAP_PHASE;        
        outputs[i] = (GGSynthFloat) sin( phase * PI ) * amplitude;
    }
}

/***********************************************************************/
void Unit_SineOscillator::gen( GGSynthFloat *amplitudes, GGSynthFloat frequency )
{
    int i;
    GGSynthFloat phaseInc = FREQ_TO_PHASEINC( frequency );
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        INCR_WRAP_PHASE;        
        outputs[i] = (GGSynthFloat) sin( phase * PI ) * amplitudes[i];
    }
}

/***********************************************************************/
void Unit_SineOscillator::gen( GGSynthFloat amplitude, GGSynthFloat *frequencies )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        GGSynthFloat phaseInc = FREQ_TO_PHASEINC( frequencies[i] );
        INCR_WRAP_PHASE;        
        outputs[i] = (GGSynthFloat) sin( phase * PI ) * amplitude;
    }
}

/***********************************************************************/
void Unit_SineOscillator::gen( GGSynthFloat *amplitudes, GGSynthFloat *frequencies )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        GGSynthFloat phaseInc = FREQ_TO_PHASEINC( frequencies[i] );
        INCR_WRAP_PHASE;        
        outputs[i] = (GGSynthFloat) sin( phase * PI ) * amplitudes[i];
    }
}
