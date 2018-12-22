#ifndef _GAMEGEMS3_UNIT_SLEWRATELIMITER_H
#define _GAMEGEMS3_UNIT_SLEWRATELIMITER_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_SlewRateLimiter moves toward its input no faster than a given rate.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_SlewRateLimiter : public UnitGen
{
public:
	Unit_SlewRateLimiter();
	~Unit_SlewRateLimiter();

/* Only calculate one value for control rate synthesis. */
    virtual void gen1( GGSynthFloat input );

/* Calculate block of values. */
    virtual void gen( GGSynthFloat input );
    virtual void gen( GGSynthFloat *inputs );

    virtual void setRate( GGSynthFloat perSecond );

private:
protected:
    GGSynthFloat value;
    GGSynthFloat increment;
};

#endif /* _GAMEGEMS3_UNIT_SLEWRATELIMITER_H */
