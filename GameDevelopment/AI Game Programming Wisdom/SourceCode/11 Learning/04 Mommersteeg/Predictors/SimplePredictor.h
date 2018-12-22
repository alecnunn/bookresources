//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    SimplePredictor.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
//	A simple O(N^2) string-matching predictor (where N is the size of the window)
//----------------------------------------------------------------------------------------------

#ifndef __SIMPLEPREDICTOR_H
#define __SIMPLEPREDICTOR_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "predictor.h"
#include "slidingwindow.h"

//----------------------------------------------------------------------------------------------
// CSimplePredictor: predicts the next element in a sequence using string-matching prediction
//----------------------------------------------------------------------------------------------

class CSimplePredictor : public CPredictor {
public:
	CSimplePredictor() { MinPerformance = 1; }

	// implementation of interface CPredictor
	virtual void			Update(int NextElement)	 { window.Add(NextElement); }
	virtual bool			GetPrediction(int &Prediction);
	virtual void			Reset() { window.SetSize(WindowSize); }

	// configuration methods
	void					Setup(int nWindowSize, int nMinPerformance);

protected:

	CSlidingWindow <int>	window;
	int						WindowSize;
	int						MinPerformance;

};

//----------------------------------------------------------------------------------------------
#endif //__SIMPLEPREDICTOR_H

