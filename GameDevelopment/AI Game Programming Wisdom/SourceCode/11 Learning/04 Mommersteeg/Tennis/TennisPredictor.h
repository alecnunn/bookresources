//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    TennisPredictor.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// This predictor is a standard string-matching predictor which can export its window contents. 
// The AI bot uses this to paint some additional information on the screen (to show how the 
// sequential prediction works internally).
//----------------------------------------------------------------------------------------------

#ifndef __TENNISPREDICTOR_H
#define __TENNISPREDICTOR_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stringmatchpredictor.h"

//----------------------------------------------------------------------------------------------
// CTennisPredictor: string-matching predictor that can export its internal window
//----------------------------------------------------------------------------------------------

class CTennisPredictor : public CStringMatchPredictor {
public:
	int						GetWindowSize();
	int						GetWindowEntry(int i);
	int						GetMaxMatchSize();
	int						GetMaxMatchPosition();
};

//----------------------------------------------------------------------------------------------
#endif // __TENNISPREDICTOR_H
