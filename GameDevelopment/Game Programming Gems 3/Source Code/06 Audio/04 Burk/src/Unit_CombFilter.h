#ifndef _GAMEGEMS3_UNIT_COMBFILTER_H
#define _GAMEGEMS3_UNIT_COMBFILTER_H

#include "ggsynth.h"
#include "Unit_Delay.h"

/**
 *	Unit_CombFilter. Delay with feedback.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_CombFilter : public Unit_Delay
{
public:
	Unit_CombFilter();
	Unit_CombFilter( double seconds );
	~Unit_CombFilter();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat *inputs );

    // Delay should be between 0.0 and 1.0
    virtual void setGain( GGSynthFloat gain );

protected:
    GGSynthFloat  gain;
};

#endif /* _GAMEGEMS3_UNIT_COMBFILTER_H */
