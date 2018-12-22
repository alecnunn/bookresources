#include <math.h>
#include "ggsynth.h"
#include "Circuit_Wind.h"

/**
 *	Circuit_Winds uses a RedNoise modulated ringing LowpassFIlter
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Circuit_Wind::Circuit_Wind()
{
    modRate = 0.6f;
    modDepth = 350.0f;
    frequency = 1000.0f;
    Q = 16.0f;
    envelope.setRate( 0.3f );
    outputs = filter.outputs;
}
	
Circuit_Wind::~Circuit_Wind()
{
}

/***********************************************************************/
void Circuit_Wind::gen( GGSynthFloat amplitude )
{
    envelope.gen1( amplitude * 0.2f );
    GGSynthFloat level = envelope.outputs[0];

    // save some CPU cycles if integrator at bottom
    if( level > 0.0f )
    {
    // Calculate LFO.
        whiteNoise.gen( level );

        redNoise.gen1( modDepth, modRate );

    // Use control rate LFO signal to modulate filter frequency.
        filter.calculateCoefficients( redNoise.outputs[0] + frequency, Q );

    // Calculate filter.
	    filter.gen( whiteNoise.outputs );

        outputs = filter.outputs;
    }
    else
    {
        outputs = envelope.outputs;  // silent
    }

}
