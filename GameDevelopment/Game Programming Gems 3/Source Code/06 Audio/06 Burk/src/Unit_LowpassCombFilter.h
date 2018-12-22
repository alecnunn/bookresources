#ifndef _GAMEGEMS3_UNIT_LOWPASSCOMBFILTER_H
#define _GAMEGEMS3_UNIT_LOWPASSCOMBFILTER_H

#include "ggsynth.h"
#include "Unit_CombFilter.h"

/**
 *	Unit_LowpassCombFilter. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_LowpassCombFilter : public Unit_CombFilter
{
public:
	Unit_LowpassCombFilter();
	Unit_LowpassCombFilter( double seconds );
	~Unit_LowpassCombFilter();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat *inputs );

/* Control feedback. Both gain and highcut must be gbetween 0.0 and 1.0
 * When highCut is 0.0 then this unit behaves just like a Unit_CombDelay.
 */
    virtual void setGain( GGSynthFloat gain, GGSynthFloat highCut );

protected:
    GGSynthFloat   gain1;
    GGSynthFloat   gain2;
    GGSynthFloat   zn1;   /* one sample delay */
};

#endif /* _GAMEGEMS3_UNIT_LOWPASSCOMBFILTER_H */
