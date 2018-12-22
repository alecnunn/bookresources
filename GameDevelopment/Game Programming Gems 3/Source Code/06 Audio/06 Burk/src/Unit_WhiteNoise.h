#ifndef _GAMEGEMS3_UNIT_WHITENOISE_H
#define _GAMEGEMS3_UNIT_WHITENOISE_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_WhiteNoise generates noise using linear congruential algorithm. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_WhiteNoise : public UnitGen
{
public:
	Unit_WhiteNoise();
	~Unit_WhiteNoise();

    unsigned long randomULong( void );
    float randomFloat( void );
    virtual void gen( GGSynthFloat amplitude );
    virtual void gen( GGSynthFloat *amplitudes );


private:
protected:
    unsigned long randSeed;
};

#endif /* _GAMEGEMS3_UNIT_WHITENOISE_H */
