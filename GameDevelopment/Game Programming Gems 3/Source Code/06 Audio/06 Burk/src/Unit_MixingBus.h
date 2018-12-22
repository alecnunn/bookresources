#ifndef _GAMEGEMS3_UNIT_MIXINGBUS_H
#define _GAMEGEMS3_UNIT_MIXINGBUS_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_MixingBus. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_MixingBus : public UnitGen
{
public:
	Unit_MixingBus();
	~Unit_MixingBus();

    // clear() must be called every loop before mixing together sounds
    virtual void clear( void );

/* Define functions for various scalar or vector input ocmbinations. */
    virtual void gen( GGSynthFloat *inputs );
    virtual void gen( GGSynthFloat *inputs, GGSynthFloat amplitude );
    virtual void gen( GGSynthFloat *inputs, GGSynthFloat *amplitudes );

private:
protected:

};

#endif /* _GAMEGEMS3_UNIT_MIXINGBUS_H */
