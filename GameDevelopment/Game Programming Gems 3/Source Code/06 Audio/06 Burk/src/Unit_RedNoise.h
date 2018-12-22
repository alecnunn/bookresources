#ifndef _GAMEGEMS3_UNIT_REDNOISE_H
#define _GAMEGEMS3_UNIT_REDNOISE_H

#include "ggsynth.h"
#include "UnitGen.h"
#include "Unit_WhiteNoise.h"

/**
 *	Unit_RedNoise generates noise using linear congruential algorithm.
 *  It increments phase like an oscillator and interpolates between
 *  succesive random values.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_RedNoise : public Unit_WhiteNoise
{
public:
	Unit_RedNoise();
	~Unit_RedNoise();

/* Only calculate one value for control rate synthesis. */
    virtual void gen1( GGSynthFloat amplitude, GGSynthFloat frequency );
/* Calculate block of values. */
    virtual void gen( GGSynthFloat amplitude, GGSynthFloat frequency );
    virtual void gen( GGSynthFloat *amplitudes, GGSynthFloat *frequencies );

private:
protected:
    GGSynthFloat phase;
    GGSynthFloat source;
    GGSynthFloat target;
    GGSynthFloat delta;
};

#endif /* _GAMEGEMS3_UNIT_REDNOISE_H */
