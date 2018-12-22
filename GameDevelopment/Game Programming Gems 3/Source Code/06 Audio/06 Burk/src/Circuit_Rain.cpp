#include <math.h>
#include "ggsynth.h"
#include "Circuit_Rain.h"

/**
 *	Circuit_Rain uses RainPloppers diffused with reverberation.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Circuit_Rain::Circuit_Rain()
{
    reverb = new Circuit_Reverb( 0.200 );
    // determines amount of feedback
    reverb->setGain( 0.91f, 0.2f );
    wetGain = 0.20f;

    leftOutputs = leftAdder.outputs;
    rightOutputs = rightAdder.outputs;
}
	
Circuit_Rain::~Circuit_Rain()
{
    delete reverb;
}

/***********************************************************************/
void Circuit_Rain::gen( GGSynthFloat amplitude, GGSynthFloat rate )
{
    leftRain.gen( amplitude, rate );
    rightRain.gen( amplitude, rate );

//. mix two ploppers and add to reverb
    adder.gen( leftRain.outputs, rightRain.outputs );
    reverb->gen( adder.outputs );
    reverbMultiplier.gen( reverb->outputs, wetGain );

// combine left and right ploppers with reverb
    leftAdder.gen( leftRain.outputs, reverbMultiplier.outputs );
    rightAdder.gen( rightRain.outputs, reverbMultiplier.outputs );
}
