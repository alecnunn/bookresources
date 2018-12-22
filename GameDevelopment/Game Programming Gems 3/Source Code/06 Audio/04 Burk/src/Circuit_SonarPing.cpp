#include <math.h>
#include "ggsynth.h"
#include "Circuit_SonarPing.h"

/**
 *	Circuit_SonarPings uses a RedNoiseModulated ringing LowpassFIlter
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Circuit_SonarPing::Circuit_SonarPing()
{
    rate = 0.4f;
//    outputs = filter.outputs;
//    outputs = redNoise.outputs;
//    outputs = pulser.outputs;
   outputs = multiplier.outputs;
}
	
Circuit_SonarPing::~Circuit_SonarPing()
{
}

/***********************************************************************/
void Circuit_SonarPing::gen( GGSynthFloat amplitude, GGSynthFloat frequency )
{
/* Calculate LFO. */
    pulser.gen( 7.0f, rate );

    redNoise.gen( 80.0f, 180.0f );

/* Use control rate LFO signal to modulate filter frequency. */
    filter.calculateCoefficients( redNoise.outputs[0] + frequency, 1000.0f );

/* Calculate filter. */
	filter.gen( pulser.outputs );

/* Scale output of filter. */
	multiplier.gen( filter.outputs, amplitude  );

}
