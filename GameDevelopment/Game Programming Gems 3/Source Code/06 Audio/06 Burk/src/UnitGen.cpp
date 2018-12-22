#include "ggsynth.h"
#include "UnitGen.h"
/**
 *	UnitGens are functional building blocks for doing audio synthesis.
 *	UnitGens can include oscillators, filters, multipliers, etc. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

GGSynthFloat UnitGen::sampleRate = 44100.0f;
GGSynthFloat UnitGen::samplePeriod = 1.0f / 44100.0f;

UnitGen::UnitGen()
{
// clear outputs to avoid big pops
    for( int i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ ) outputs[i] = 0.0f;
}
	
UnitGen::~UnitGen()
{
}
