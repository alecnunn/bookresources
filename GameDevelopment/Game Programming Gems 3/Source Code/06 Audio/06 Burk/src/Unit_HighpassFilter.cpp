#include <math.h>

#include "ggsynth.h"
#include "Unit_HighpassFilter.h"

/**
 *	Unit_HighpassFilter passes high frequencies and cuts low frequencies. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_HighpassFilter::Unit_HighpassFilter()
{
        calculateCoefficients( 200.0f, 1.0f );
}
	
Unit_HighpassFilter::~Unit_HighpassFilter()
{
}

/*********************************************************************************
** Calculate coefficients for Highpass filter.
*/
void Unit_HighpassFilter::calculateCoefficients( GGSynthFloat frequency, GGSynthFloat Q )
{
	GGSynthFloat    scalar, opc;

	if( frequency  < BIQUAD_MIN_FREQ )  frequency  = BIQUAD_MIN_FREQ;
	if( Q < BIQUAD_MIN_Q ) Q = BIQUAD_MIN_Q;

	calcCommon( frequency * UnitGen.samplePeriod, Q );

	scalar = 1.0f / (1.0f + alpha);
	opc = (1.0f + cos_omega);

	a0 = opc * 0.5f * scalar;
	a1 =  - opc * scalar;
    a2 = a0;
	b1 = -2.0f * cos_omega * scalar;
	b2 = (1.0f - alpha) * scalar;
}
