#ifndef _GAMEGEMS3_UNIT_INTEGRATOR_H
#define _GAMEGEMS3_UNIT_INTEGRATOR_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_Integrator integrates its input and clips it within limits.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_Integrator : public UnitGen
{
public:
	Unit_Integrator();
	~Unit_Integrator();

    GGSynthFloat minimum;
    GGSynthFloat maximum;
    GGSynthFloat value;

/* Only calculate one value for control rate synthesis. */
    virtual void gen1( GGSynthFloat input );
/* Calculate block of values. */
    virtual void gen( GGSynthFloat input );
    virtual void gen( GGSynthFloat *inputs );

    virtual GGSynthFloat calculateIncrement( GGSynthFloat seconds );

private:
protected:
};

#endif /* _GAMEGEMS3_UNIT_INTEGRATOR_H */
