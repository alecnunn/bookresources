#ifndef _GAMEGEMS3_UNIT_BANDPASSFILTER_H
#define _GAMEGEMS3_UNIT_BANDPASSFILTER_H

#include "ggsynth.h"
#include "Unit_BiquadFilter.h"

/**
 *	Unit_BandpassFilter cut high and low frequencies outside the band.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_BandpassFilter : public Unit_BiquadFilter
{
public:
	Unit_BandpassFilter();
	~Unit_BandpassFilter();

    virtual void calculateCoefficients( GGSynthFloat frequency, GGSynthFloat Q );

};

#endif /* _GAMEGEMS3_UNIT_BANDPASSFILTER_H */
