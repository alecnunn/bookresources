#ifndef _GAMEGEMS3_UNIT_ADD_H
#define _GAMEGEMS3_UNIT_ADD_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_Add. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_Add : public UnitGen
{
public:
	Unit_Add();
	~Unit_Add();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat *input, GGSynthFloat scalar );
    virtual void gen( GGSynthFloat *inputA, GGSynthFloat *inputB );

};

#endif /* _GAMEGEMS3_UNIT_ADD_H */
