#include <math.h>

#include "ggsynth.h"
#include "Unit_BiquadFilter.h"

/**
 *	Unit_BiquadFilter implements a second order IIR filter. 
	Here is the equation that we use for this filter:
     y(n) = a0*x(n) + a1*x(n-1)  + a2*x(n-2) - b1*y(n-1)  - b2*y(n-2)
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_BiquadFilter::Unit_BiquadFilter()
{
    xn1 = xn2 = yn1 = yn2 = (GGSynthFloat) 0.0;
    a0 = a1 = a2 = b1 = b2 = 0.0f;
}
	
Unit_BiquadFilter::~Unit_BiquadFilter()
{
}

/***********************************************************
** Calculate coefficients common to many parametric biquad filters.
*/
void Unit_BiquadFilter::calcCommon( GGSynthFloat ratio, GGSynthFloat Q )
{
	GGSynthFloat omega;

/* Don't let frequency get too close to Nyquist or filter will blow up. */
	if( ratio >= 0.499f ) ratio = 0.499f; 
	omega = 2.0f * (GGSynthFloat)PI * ratio;

	cos_omega = (GGSynthFloat) cos( omega );
	sin_omega = (GGSynthFloat) sin( omega );
	alpha = sin_omega / (2.0f * Q);
}

#define USE_PERMUTATION  (0)

#if USE_PERMUTATION
/**************************************************************
** Perform core IIR filter calculation with permutation.
*/
void Unit_BiquadFilter::gen( GGSynthFloat *inputs )
{
	int i;
    GGSynthFloat xn;

	for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i+=2)
	{
// Generate outputs by filtering inputs.
		xn = inputs[i];
		yn2 = (a0 * xn) + (a1 * xn1) + (a2 * xn2) - (b1 * yn1) - (b2 * yn2);
		outputs[i] = yn2;

// Permute filter operations to reduce data movement.
// Just substitute variables instead of doing xn1=xn, etc.
		xn2 = inputs[i+1];
		yn1 = (a0 * xn2) + (a1 * xn) + (a2 * xn1) - (b1 * yn2) - (b2 * yn1);
		outputs[i+1] = yn1;

// Only move a little data.
		xn1 = xn2;
		xn2 = xn;
	}

// Apply a small impulse to filter to prevent arithmetic underflow,
// which can cause the FPU to interrupt the CPU.
	yn1 += (GGSynthFloat) 1.0E-26;
}
#else
/*****************************************************************
** Perform core IIR filter calculation without permutation.
*/
void Unit_BiquadFilter::gen( GGSynthFloat *inputs )
{
	int i;
    GGSynthFloat xn, yn;

	for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++)
	{
// Generate outputs by filtering inputs.
		xn = inputs[i];
		yn = (a0 * xn) + (a1 * xn1) + (a2 * xn2) - (b1 * yn1) - (b2 * yn2);
		outputs[i] = yn;

// Delay input and output values.
        xn2 = xn1;
        xn1 = xn;
        yn2 = yn1;
        yn1 = yn;
	}

// Apply a small bipolar impulse to filter to prevent arithmetic underflow,
// which can cause the FPU to interrupt the CPU.
	yn1 += (GGSynthFloat) 1.0E-26;
	yn2 -= (GGSynthFloat) 1.0E-26;
}
#endif