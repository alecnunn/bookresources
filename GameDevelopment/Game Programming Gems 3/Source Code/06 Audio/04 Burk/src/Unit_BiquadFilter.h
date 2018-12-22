#ifndef _GAMEGEMS3_UNIT_BIQUADFILTER_H
#define _GAMEGEMS3_UNIT_BIQUADFILTER_H

#include "ggsynth.h"
#include "Unit_Filter.h"

/**
 *	Unit_BiquadFilter implements a second order IIR filter. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

#define BIQUAD_MIN_FREQ      (0.00001f) // REVIEW
#define BIQUAD_MIN_Q         (0.00001f) // REVIEW

class Unit_BiquadFilter : public Unit_Filter
{
public:
	Unit_BiquadFilter();
	~Unit_BiquadFilter();

    virtual void calculateCoefficients( GGSynthFloat frequency, GGSynthFloat Q ) = 0;

    virtual void gen( GGSynthFloat *input );

private:

protected:
    void   calcCommon( GGSynthFloat ratio, GGSynthFloat Q );

    GGSynthFloat      xn1;    // storage for delayed signals
	GGSynthFloat      xn2;
	GGSynthFloat      yn1;
	GGSynthFloat      yn2;

	GGSynthFloat      a0;    // oefficients
	GGSynthFloat      a1;
	GGSynthFloat      a2;

	GGSynthFloat      b1;
	GGSynthFloat      b2;

	GGSynthFloat      cos_omega;
	GGSynthFloat      sin_omega;
	GGSynthFloat      alpha;
};

#endif /* _GAMEGEMS3_UNIT_BIQUADFILTER_H */
