#include <math.h>
#include "ggsynth.h"
#include "Unit_Add.h"

/**
 *	Unit_Add adds its inputs together.
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_Add::Unit_Add()
{
}
	
Unit_Add::~Unit_Add()
{
}

/***********************************************************************/
void Unit_Add::gen( GGSynthFloat *inputs, GGSynthFloat scalar )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = inputs[i] + scalar;
    }
}

void Unit_Add::gen( GGSynthFloat *inputsA, GGSynthFloat *inputsB )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = inputsA[i] + inputsB[i];
    }
}
