#include <math.h>
#include "ggsynth.h"
#include "Circuit_Rocket.h"

/**
 *	Circuit_Rockets uses a RedNoise modulated RedNoise
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Circuit_Rocket::Circuit_Rocket()
{
    modRate = 731.0f;
    modDepth = 600.0f;
    frequency = 1300.0f;
    envelope.setRate( 5.0f );
    outputs = redNoise2.outputs;
}
	
Circuit_Rocket::~Circuit_Rocket()
{
}

/***********************************************************************/
void Circuit_Rocket::gen( GGSynthFloat amplitude )
{
    envelope.gen1( amplitude );

    GGSynthFloat level = envelope.outputs[0];

    // save some CPU cycles if integrator at bottom
    if( level > 0.0 )
    {
        redNoise1.gen1( modDepth, modRate );

        redNoise2.gen( level, redNoise1.outputs[0] + frequency );
    }
}
