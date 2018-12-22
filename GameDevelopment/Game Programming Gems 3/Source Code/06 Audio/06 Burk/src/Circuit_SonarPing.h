#ifndef _GAMEGEMS3_CIRCUIT_SONARPING_H
#define _GAMEGEMS3_CIRCUIT_SONARPING_H

#include "ggsynth.h"
#include "Unit_ImpulseOscillator.h"
#include "Unit_LowpassFilter.h"
#include "Unit_RedNoise.h"
#include "Unit_Multiply.h"

/**
 *	Circuit_SonarPing. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Circuit_SonarPing
{
public:
    GGSynthFloat  rate;

    GGSynthFloat *outputs;

	Circuit_SonarPing();
	~Circuit_SonarPing();

/* Define functions for various scalar or vector input ocmbinations. */
    virtual void gen( GGSynthFloat frequency, GGSynthFloat amplitude );

private:
protected:

    Unit_ImpulseOscillator pulser;
    Unit_RedNoise          redNoise;
    Unit_LowpassFilter     filter;
    Unit_Multiply          multiplier;

};

#endif /* _GAMEGEMS3_CIRCUIT_SONARPING_H */
