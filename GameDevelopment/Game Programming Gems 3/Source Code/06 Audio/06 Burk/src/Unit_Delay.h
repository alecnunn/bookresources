#ifndef _GAMEGEMS3_UNIT_DELAY_H
#define _GAMEGEMS3_UNIT_DELAY_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_Delay. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_Delay : public UnitGen
{
public:
	Unit_Delay();
	Unit_Delay( double seconds );
	~Unit_Delay();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat *inputs );

protected:
    virtual void init( double seconds );

    GGSynthFloat *delayLine;
    int           numFrames;
    int           cursor;
};

#endif /* _GAMEGEMS3_UNIT_DELAY_H */
