#ifndef _GAMEGEMS3_UNIT_ALLPASSDELAY_H
#define _GAMEGEMS3_UNIT_ALLPASSDELAY_H

#include "ggsynth.h"
#include "Unit_CombFilter.h"

/**
 *	Unit_AllpassDelay. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_AllpassDelay : public Unit_CombFilter
{
public:
	Unit_AllpassDelay();
	Unit_AllpassDelay( double seconds );
	~Unit_AllpassDelay();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat *input );

    virtual void setGain( GGSynthFloat gain );

private:
    GGSynthFloat  omg2; /* 1 - gain^2 */
};

#endif /* _GAMEGEMS3_UNIT_ALLPASSDELAY_H */
