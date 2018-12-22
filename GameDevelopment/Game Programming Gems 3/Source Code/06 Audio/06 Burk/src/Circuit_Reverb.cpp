#include <math.h>
#include "ggsynth.h"
#include "Circuit_Reverb.h"

/**
 *	Circuit_Reverbs uses bank of CombFilters
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Circuit_Reverb::Circuit_Reverb( double baseDelay )
{
    init( baseDelay );
    outputs = allpassDelay->outputs;
}
	
Circuit_Reverb::~Circuit_Reverb()
{
    int i;
    for( i=0; i<REVERB_NUM_COMB_FILTERS; i++ )
    {
        delete combFilters[i];
    }
    delete allpassDelay;
}

void Circuit_Reverb::init( double baseDelay )
{
// Set up comb filters with staggered delay times.
    combFilters[0] = new Unit_LowpassCombFilter( baseDelay );
    combFilters[1] = new Unit_LowpassCombFilter( 1.12 * baseDelay );
    combFilters[2] = new Unit_LowpassCombFilter( 1.22 * baseDelay );
    combFilters[3] = new Unit_LowpassCombFilter( 1.37 * baseDelay );
    combFilters[4] = new Unit_LowpassCombFilter( 1.44 * baseDelay );
    combFilters[5] = new Unit_LowpassCombFilter( 1.57 * baseDelay );

    double allDelay = 0.12 * baseDelay;
    if( allDelay < 0.006 ) allDelay = 0.006;
    allpassDelay = new Unit_AllpassDelay( allDelay );
}

void Circuit_Reverb::setGain( GGSynthFloat gain, GGSynthFloat highCut )
{
    for( int i=0; i<REVERB_NUM_COMB_FILTERS; i++ )
    {
        combFilters[i]->setGain( gain, highCut );
    }
    allpassDelay->setGain( 0.7f );
}

/***********************************************************************/
void Circuit_Reverb::gen( GGSynthFloat *inputs )
{
// Clear mixer busses before adding together signals.
    mixer.clear();

// Run each comb filter and mix outputs.
    for( int i=0; i<REVERB_NUM_COMB_FILTERS; i++ )
    {
        combFilters[i]->gen( inputs );
        mixer.gen( combFilters[i]->outputs );
    }

    allpassDelay->gen( mixer.outputs );

}
