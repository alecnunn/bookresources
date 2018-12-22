#include <math.h>
#include "ggsynth.h"
#include "Circuit_RainPlops.h"

/**
 *	Circuit_RainPlopss uses a RedNoise modulated ringing LowpassFIlter
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Circuit_RainPlops::Circuit_RainPlops()
{
    modRate = 9.9f;
    modDepth = 90.0f;
    frequency = 250.0f;
    Q = 2.0f;
    outputs = filter.outputs;
}
	
Circuit_RainPlops::~Circuit_RainPlops()
{
}

/***********************************************************************/
void Circuit_RainPlops::gen( GGSynthFloat amplitude, GGSynthFloat rate )
{
// random triggers, boost gain because filter has quiet response to impulses
    trigger.gen( amplitude * 15.0f, rate );

    redNoise.gen1( modDepth, modRate );

// Use control rate LFO signal to modulate filter frequency.
    filter.calculateCoefficients( redNoise.outputs[0] + frequency, Q );

// Calculate filter.
	filter.gen( trigger.outputs );
}
