#ifndef _GAMEGEMS3_UNIT_FILTER_H
#define _GAMEGEMS3_UNIT_FILTER_H

#include "ggsynth.h"
#include "UnitGen.h"

/**
 *	Unit_Filter modifies the spectrum of its input. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_Filter : public UnitGen
{
public:
	Unit_Filter();
	~Unit_Filter();

    virtual void Unit_Filter::gen( GGSynthFloat *inputs ) = 0;

private:
protected:
};

#endif /* _GAMEGEMS3_UNIT_FILTER_H */
