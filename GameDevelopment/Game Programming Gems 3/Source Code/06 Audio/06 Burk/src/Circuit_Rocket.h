#ifndef _GAMEGEMS3_CIRCUIT_ROCKET_H
#define _GAMEGEMS3_CIRCUIT_ROCKET_H

#include "ggsynth.h"
#include "Unit_SlewRateLimiter.h"
#include "Unit_RedNoise.h"

/**
 *	Circuit_Rocket. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Circuit_Rocket
{
public:
    GGSynthFloat  modRate;
    GGSynthFloat  modDepth;
    GGSynthFloat  frequency;

    Unit_RedNoise          redNoise1;
    Unit_RedNoise          redNoise2;
    Unit_SlewRateLimiter   envelope;

    GGSynthFloat *outputs;

	Circuit_Rocket();
	~Circuit_Rocket();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat amplitude );

private:
protected:
};

#endif /* _GAMEGEMS3_CIRCUIT_ROCKET_H */
