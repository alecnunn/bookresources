#ifndef _GAMEGEMS3_UNIT_INTERPDELAY_H
#define _GAMEGEMS3_UNIT_INTERPDELAY_H

#include "ggsynth.h"
#include "Unit_Delay.h"

/**
 *	Unit_InterpolatingDelay. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_InterpolatingDelay : public Unit_Delay
{
public:
	Unit_InterpolatingDelay();
	Unit_InterpolatingDelay( double seconds );
	~Unit_InterpolatingDelay();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat *inputs, GGSynthFloat *delays );

};

#endif /* _GAMEGEMS3_UNIT_INTERPDELAY_H */
