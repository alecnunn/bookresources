#include <math.h>
#include "ggsynth.h"
#include "Unit_Multiply.h"

/**
 *	Unit_Multiply multiplys two values. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

Unit_Multiply::Unit_Multiply()
{
}
	
Unit_Multiply::~Unit_Multiply()
{
}

/***********************************************************************/
void Unit_Multiply::gen( GGSynthFloat *inputs, GGSynthFloat scalar )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = inputs[i] * scalar;
    }
}

void Unit_Multiply::gen( GGSynthFloat *inputsA, GGSynthFloat *inputsB )
{
    int i;
    for( i=0; i<GGSYNTH_FRAMES_PER_BLOCK; i++ )
    {
        outputs[i] = inputsA[i] * inputsB[i];
    }
}
