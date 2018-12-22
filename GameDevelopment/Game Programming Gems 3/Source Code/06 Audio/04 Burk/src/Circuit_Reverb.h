#ifndef _GAMEGEMS3_CIRCUIT_REVERB_H
#define _GAMEGEMS3_CIRCUIT_REVERB_H

#include "ggsynth.h"
#include "Unit_MixingBus.h"
#include "Unit_LowpassCombFilter.h"
#include "Unit_AllpassDelay.h"

/**
 *	Circuit_Reverb. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

#define REVERB_NUM_COMB_FILTERS  (6)

class Circuit_Reverb
{
public:
    Unit_MixingBus   mixer;
    Unit_LowpassCombFilter *combFilters[6];
    Unit_AllpassDelay *allpassDelay;

    GGSynthFloat *outputs;

	Circuit_Reverb( double baseDelay );
	~Circuit_Reverb();

    virtual void gen( GGSynthFloat *inputs );

    virtual void setGain( GGSynthFloat gain, GGSynthFloat highCut );

private:
protected:
    virtual void init( double baseDelay );

};

#endif /* _GAMEGEMS3_CIRCUIT_REVERB_H */
