#ifndef _GAMEGEMS3_CIRCUIT_HELICOPTER_H
#define _GAMEGEMS3_CIRCUIT_HELICOPTER_H

#include "ggsynth.h"
#include "Unit_WhiteNoise.h"
#include "Unit_LowpassFilter.h"
#include "Unit_SineOscillator.h"
#include "Unit_Multiply.h"
#include "Unit_Add.h"
#include "Unit_SlewRateLimiter.h"
#include "Unit_InterpolatingDelay.h"

/**
 *	Circuit_Helicopter. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Circuit_Helicopter
{
public:
    GGSynthFloat  modRate;
    GGSynthFloat  modDepth;
    GGSynthFloat  midDelay;
    GGSynthFloat  frequency;
    GGSynthFloat  Q;

    Unit_SlewRateLimiter     envelope;
    Unit_WhiteNoise          whiteNoise;
    Unit_SineOscillator      sinOsc;
    Unit_LowpassFilter       filter;
    Unit_Add                 adder1;
    Unit_Add                 adder2;
    Unit_InterpolatingDelay *delay1;
    Unit_InterpolatingDelay *delay2;
    Unit_Multiply            inverter;
    Unit_Add                 mixer;

    GGSynthFloat *outputs;

	Circuit_Helicopter( GGSynthFloat rotorDiameterInFeet );
	~Circuit_Helicopter();

/* Define functions for various scalar or vector input combinations. */
    virtual void gen( GGSynthFloat amplitude );

private:
protected:

};

#endif /* _GAMEGEMS3_CIRCUIT_HELICOPTER_H */
