#ifndef _GAMEGEMS3_CIRCUIT_WIND_H
#define _GAMEGEMS3_CIRCUIT_WIND_H

#include "ggsynth.h"
#include "Unit_WhiteNoise.h"
#include "Unit_LowpassFilter.h"
#include "Unit_RedNoise.h"
#include "Unit_SlewRateLimiter.h"

/**
 *	Circuit_Wind.
 * Generate wind sounds by filtering white noise.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Circuit_Wind
{
public:
    GGSynthFloat  modRate;
    GGSynthFloat  modDepth;
    GGSynthFloat  frequency;
    GGSynthFloat  Q;

    Unit_WhiteNoise        whiteNoise;
    Unit_RedNoise          redNoise;
    Unit_LowpassFilter     filter;
    Unit_SlewRateLimiter   envelope;

    GGSynthFloat *outputs;

	Circuit_Wind();
	~Circuit_Wind();

/* Define functions for various scalar or vector input ocmbinations. */
    virtual void gen( GGSynthFloat amplitude );

private:
protected:

};

#endif /* _GAMEGEMS3_CIRCUIT_WIND_H */
