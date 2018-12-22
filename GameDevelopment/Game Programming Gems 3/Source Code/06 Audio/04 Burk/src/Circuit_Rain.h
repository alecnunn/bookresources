#ifndef _GAMEGEMS3_CIRCUIT_RAIN_H
#define _GAMEGEMS3_CIRCUIT_RAIN_H

#include "ggsynth.h"
#include "Circuit_RainPlops.h"
#include "Unit_Add.h"
#include "Unit_Multiply.h"
#include "Circuit_Reverb.h"

/**
 *	Circuit_Rain. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Circuit_Rain
{
public:

    Circuit_RainPlops   leftRain;
    Circuit_RainPlops   rightRain;
    Unit_Add            adder;
    Unit_Add            leftAdder;
    Unit_Add            rightAdder;
    Unit_Multiply       reverbMultiplier;
    Circuit_Reverb     *reverb;

    GGSynthFloat        wetGain;
    GGSynthFloat       *leftOutputs;
    GGSynthFloat       *rightOutputs;

	Circuit_Rain();
	~Circuit_Rain();

/* Define functions for various scalar or vector input ocmbinations. */
    virtual void gen( GGSynthFloat amplitude, GGSynthFloat rate );

private:
protected:

};

#endif /* _GAMEGEMS3_CIRCUIT_RAIN_H */
