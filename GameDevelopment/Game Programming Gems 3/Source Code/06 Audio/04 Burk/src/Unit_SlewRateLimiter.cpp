#include <math.h>
#include "ggsynth.h"
#include "Unit_SlewRateLimiter.h"

/**
 *	Unit_SlewRateLimiter moves toward its input no faster than a given rate.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_SlewRateLimiter::Unit_SlewRateLimiter()
{
    value = 0.0f;
    setRate( 1.0f );
}
	
Unit_SlewRateLimiter::~Unit_SlewRateLimiter()
{
}

void Unit_SlewRateLimiter::setRate( GGSynthFloat perSecond )
{
    increment = perSecond * UnitGen::samplePeriod;
}

/* Move toward input no faster than the increment allows. */
#define SLEW_RATE_LIMITER( incr ) \
    if( input > value ) \
    { \
        value += (incr); \
        if( value > input ) value = input; \
    } \
    else if( input < value ) \
    { \
        value -= (incr); \
        if( value < input ) value = input; \
    }

/* Only calculate one value for control rate synthesis. */
void Unit_SlewRateLimiter::gen1( GGSynthFloat input )
{
    SLEW_RATE_LIMITER( increment * GGSYNTH_FRAMES_PER_BLOCK);
    outputs[0] = value;
}

void Unit_SlewRateLimiter::gen( GGSynthFloat input )
{
    for( int i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        SLEW_RATE_LIMITER( increment );
        outputs[i] = value;
    }
}

void Unit_SlewRateLimiter::gen( GGSynthFloat *inputs )
{
    for( int i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        GGSynthFloat input = inputs[i];
        SLEW_RATE_LIMITER( increment );
        outputs[i] = value;
    }
}
