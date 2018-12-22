#include <math.h>
#include "ggsynth.h"
#include "Circuit_Helicopter.h"

/**
 * Circuit_Helicopters uses a LowPass filtered white noise passed through
 * two modulated delay line for the Doppler shift effect.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Circuit_Helicopter::Circuit_Helicopter( GGSynthFloat rotorDiameterInFeet )
{
    modRate   = 320.0f / 60;  // cruising PRM for UH-1 Huey
/* Difference in arrival time for rotor sound at closest and farthest point
 * is diameter / speedOfSound. The modDepth should be set 1/2 that peak to peak value.
 */
    modDepth  = rotorDiameterInFeet / (1100 * 2);
/* make delay lines slightly larger than needed */
    midDelay = modDepth * 1.01f;

    frequency = 292.0f;
    Q         = 6.0f;
    envelope.setRate( 0.3f );

    // make a doppler shifter for each rotor
    delay1 = new Unit_InterpolatingDelay(midDelay * 2.0f);
    delay2 = new Unit_InterpolatingDelay(midDelay * 2.0f);

    // Set Helicopter frequency once..
    filter.calculateCoefficients( frequency, Q );

    outputs = mixer.outputs;
}

Circuit_Helicopter::~Circuit_Helicopter()
{
    delete delay1;
    delete delay2;
}

/***********************************************************************/
void Circuit_Helicopter::gen( GGSynthFloat amplitude )
{
    envelope.gen1( amplitude );


// Calculate whoosh.
    whiteNoise.gen( envelope.outputs[0] );
	filter.gen( whiteNoise.outputs );

// calculate rotor modulation
    sinOsc.gen( modDepth, modRate );
    inverter.gen( sinOsc.outputs, -1.0f );

// apply doppler delays 180 degrees out of phase
    adder1.gen( sinOsc.outputs, midDelay );
    delay1->gen( filter.outputs, adder1.outputs );

    adder2.gen( inverter.outputs, midDelay );
    delay2->gen( filter.outputs, adder2.outputs );

// mix two rotors
    mixer.gen( delay1->outputs, delay2->outputs );
}
