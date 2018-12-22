#ifndef _GAMEGEMS3_UNITGEN_H
#define _GAMEGEMS3_UNITGEN_H

#include "ggsynth.h"

/**
 *	UnitGens are functional building blocks for doing audio synthesis.
 *	UnitGens can include oscillators, filters, multipliers, etc. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class UnitGen
{
public:
	UnitGen();
	~UnitGen();

// calculated sample will be placed in this array
    GGSynthFloat   outputs[GGSYNTH_FRAMES_PER_BLOCK];

    static GGSynthFloat sampleRate;
    static GGSynthFloat samplePeriod;

static void UnitGen::setSampleRate( GGSynthFloat sampleRate )
{
    UnitGen::sampleRate = sampleRate;
    UnitGen::samplePeriod = 1.0f / sampleRate;
}

static double UnitGen::getSampleRate( void )
{
    return UnitGen::sampleRate;
}

};

#endif /* _GAMEGEMS3_UNITGEN_H */
