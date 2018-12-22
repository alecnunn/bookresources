#ifndef _GAMEGEMS3_CIRCUIT_RAINPLOPS_H
#define _GAMEGEMS3_CIRCUIT_RAINPLOPS_H

#include "ggsynth.h"
#include "Unit_PoissonTrigger.h"
#include "Unit_LowpassFilter.h"
#include "Unit_RedNoise.h"

/**
 *	Circuit_RainPlops. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Circuit_RainPlops
{
public:
    GGSynthFloat  modRate;
    GGSynthFloat  modDepth;
    GGSynthFloat  frequency;
    GGSynthFloat  Q;

    Unit_PoissonTrigger    trigger;
    Unit_RedNoise          redNoise;
    Unit_LowpassFilter     filter;

    GGSynthFloat *outputs;

	Circuit_RainPlops();
	~Circuit_RainPlops();

/* Define functions for various scalar or vector input ocmbinations. */
    virtual void gen( GGSynthFloat amplitude, GGSynthFloat rate );

private:
protected:

};

#endif /* _GAMEGEMS3_CIRCUIT_RAINPLOPS_H */
