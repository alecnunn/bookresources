#ifndef _GAMEGEMS3_UNIT_HIGHPASSFILTER_H
#define _GAMEGEMS3_UNIT_HIGHPASSFILTER_H

#include "ggsynth.h"
#include "Unit_BiquadFilter.h"

/**
 *	Unit_HighpassFilter passes high frequencies and cuts low frequencies. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_HighpassFilter : public Unit_BiquadFilter
{
public:
	Unit_HighpassFilter();
	~Unit_HighpassFilter();

    virtual void calculateCoefficients( GGSynthFloat frequency, GGSynthFloat Q );

private:

protected:
};

#endif /* _GAMEGEMS3_UNIT_HIGHPASSFILTER_H */
