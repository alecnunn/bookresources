#ifndef _GAMEGEMS3_UNIT_POISSONTRIGGER_H
#define _GAMEGEMS3_UNIT_POISSONTRIGGER_H

#include "ggsynth.h"
#include "Unit_WhiteNoise.h"

/**
 *	Unit_PoissonTrigger generates randomly sized triggers with a Poisson distribution. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_PoissonTrigger : public Unit_WhiteNoise
{
public:
	Unit_PoissonTrigger();
	~Unit_PoissonTrigger();
// rate should be between 0.0 and 1.0
    virtual void gen( GGSynthFloat amplitude, GGSynthFloat rate );

private:
protected:
    unsigned long randSeed;
};

#endif /* _GAMEGEMS3_UNIT_POISSONTRIGGER_H */
