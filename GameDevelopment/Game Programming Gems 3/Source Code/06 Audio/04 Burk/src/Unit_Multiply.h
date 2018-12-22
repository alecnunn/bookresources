#ifndef _GAMEGEMS3_UNIT_MULTIPLY_H
#define _GAMEGEMS3_UNIT_MULTIPLY_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_Multiply. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_Multiply : public UnitGen
{
public:
	Unit_Multiply();
	~Unit_Multiply();

/* Define functions for various scalar or vector input ocmbinations. */
    virtual void gen( GGSynthFloat *inputs, GGSynthFloat scalar );
    virtual void gen( GGSynthFloat *inputsA, GGSynthFloat *inputsB );

private:
protected:

};

#endif /* _GAMEGEMS3_UNIT_MULTIPLY_H */
