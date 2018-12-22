#ifndef _GAMEGEMS3_UNIT_OSCILLATOR_H
#define _GAMEGEMS3_UNIT_OSCILLATOR_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_Oscillator.
 *  Root class for other oscillators.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

// convert frequency in Hertz to an internal phaseIncrement
#define FREQ_TO_PHASEINC(freq)   ((freq) * UnitGen::samplePeriod * 2.0f)

// oscillators all use this to generate a phaser between -1.0 and +1.0
#define INCR_WRAP_PHASE \
        phase += phaseInc; \
        if( phase >= 1.0 ) phase -= 2.0f; \
        else if( phase < -1.0 ) phase += 2.0f;


class Unit_Oscillator : public UnitGen
{
public:
	Unit_Oscillator();
	~Unit_Oscillator();

    virtual void gen( GGSynthFloat amplitude, GGSynthFloat frequency ) = 0;
    virtual void gen( GGSynthFloat *amplitudes, GGSynthFloat frequency ) = 0;
    virtual void gen( GGSynthFloat amplitude, GGSynthFloat *frequencies ) = 0;
    virtual void gen( GGSynthFloat *amplitudes, GGSynthFloat *frequencies ) = 0;

private:
protected:
    GGSynthFloat phase;

};

#endif /* _GAMEGEMS3_UNIT_OSCILLATOR_H */
