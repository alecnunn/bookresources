//#include <stdio.h>
#include "ggsynth.h"
#include "Unit_ImpulseOscillator.h"

/**
 *	Unit_ImpulseOscillators generate a single sample wide impulse train. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_ImpulseOscillator::Unit_ImpulseOscillator()
{
}
	
Unit_ImpulseOscillator::~Unit_ImpulseOscillator()
{
}

/***********************************************************************/
void Unit_ImpulseOscillator::gen( GGSynthFloat amplitude, GGSynthFloat frequency )
{
    int i;
    GGSynthFloat phaseInc = FREQ_TO_PHASEINC( frequency );

    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        phase += phaseInc;
        if( phase >= 1.0 )
        {
            outputs[i] = amplitude;
            phase -= 2.0f;
        }
        else if( phase < -1.0 )
        {
            outputs[i] = amplitude;
            phase += 2.0f;
        }
        else
        {
            outputs[i] = 0.0f;
        }
    }
}

/***********************************************************************/
void Unit_ImpulseOscillator::gen( GGSynthFloat *amplitudes, GGSynthFloat frequency )
{
    int i;
    GGSynthFloat phaseInc = FREQ_TO_PHASEINC( frequency );
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        phase += phaseInc;

        if( phase >= 1.0 )
        {
            outputs[i] = amplitudes[i];
            phase -= 2.0f;
        }
        else if( phase < -1.0 )
        {
            outputs[i] = amplitudes[i];
            phase += 2.0f;
        }
        else
        {
            outputs[i] = 0.0f;
        }
    }
}

/***********************************************************************/
void Unit_ImpulseOscillator::gen( GGSynthFloat amplitude, GGSynthFloat *frequencies )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        phase += FREQ_TO_PHASEINC( frequencies[i] );

        if( phase >= 1.0 )
        {
            outputs[i] = amplitude;
            phase -= 2.0f;
        }
        else if( phase < -1.0 )
        {
            outputs[i] = amplitude;
            phase += 2.0f;
        }
        else
        {
            outputs[i] = 0.0f;
        }
    }
}

/***********************************************************************/
void Unit_ImpulseOscillator::gen( GGSynthFloat *amplitudes, GGSynthFloat *frequencies )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        phase += FREQ_TO_PHASEINC( frequencies[i] );

        if( phase >= 1.0 )
        {
            outputs[i] = amplitudes[i];
            phase -= 2.0f;
        }
        else if( phase < -1.0 )
        {
            outputs[i] = amplitudes[i];
            phase += 2.0f;
        }
        else
        {
            outputs[i] = 0.0f;
        }
    }
}
