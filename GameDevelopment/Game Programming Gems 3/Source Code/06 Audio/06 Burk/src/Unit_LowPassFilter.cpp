#include <math.h>

#include "ggsynth.h"
#include "Unit_LowpassFilter.h"

/**
 *	Unit_LowpassFilter passes low frequencies and cuts high frequencies. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_LowpassFilter::Unit_LowpassFilter()
{
        calculateCoefficients( 200.0f, 1.0f );
}
	
Unit_LowpassFilter::~Unit_LowpassFilter()
{
}

/*********************************************************************************
** Calculate coefficients for Lowpass filter.
*/
void Unit_LowpassFilter::calculateCoefficients( GGSynthFloat frequency, GGSynthFloat Q )
{
	GGSynthFloat    scalar, omc;

	if( frequency  < BIQUAD_MIN_FREQ )  frequency  = BIQUAD_MIN_FREQ;
	if( Q < BIQUAD_MIN_Q ) Q = BIQUAD_MIN_Q;

	calcCommon( frequency * UnitGen.samplePeriod, Q );

	scalar = 1.0f / (1.0f + alpha);
	omc = (1.0f - cos_omega);
    
	a0 = omc * 0.5f * scalar;
	a1 = omc * scalar;
    a2 = a0;
	b1 = -2.0f * cos_omega * scalar;
	b2 = (1.0f - alpha) * scalar;
}
