#ifndef _GAMEGEMS3_UNIT_LOWPASSFILTER_H
#define _GAMEGEMS3_UNIT_LOWPASSFILTER_H

#include "ggsynth.h"
#include "Unit_BiquadFilter.h"

/**
 *	Unit_LowpassFilter passes low frequencies and cuts high frequencies. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_LowpassFilter : public Unit_BiquadFilter
{
public:
	Unit_LowpassFilter();
	~Unit_LowpassFilter();

    virtual void calculateCoefficients( GGSynthFloat frequency, GGSynthFloat Q );

private:

protected:
};

#endif /* _GAMEGEMS3_UNIT_LOWPASSFILTER_H */
