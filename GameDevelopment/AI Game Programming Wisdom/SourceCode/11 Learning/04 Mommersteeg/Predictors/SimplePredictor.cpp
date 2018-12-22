//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    SimplePredictor.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "SimplePredictor.h"

//----------------------------------------------------------------------------------------------
// Setup(): sets up the predictor
//----------------------------------------------------------------------------------------------

void CSimplePredictor::Setup(int nWindowSize, int nMinPerformance) { 
	// set up window
	WindowSize = nWindowSize; 
	Reset();
	MinPerformance = nMinPerformance; 
}

//----------------------------------------------------------------------------------------------
// GetPrediction(): determines the successor using string-matching prediction (complexity O^2)
//----------------------------------------------------------------------------------------------

bool CSimplePredictor::GetPrediction(int &Prediction) {
	int N = window.GetSize();
	int MaxStringSize = 0, MaxStringPosition = N-2;
	
	// if the window is empty, we are unable to make a prediction
	if (window.GetSize() > 0) {
				
		for (int i = N-2; i >= 0; i--) {
			int j = 0;
			while (i-j >= 0 && window[i-j] == window[N-1-j]) {
				j++;
			}
			if (j > MaxStringSize) {
				MaxStringPosition = i;
				MaxStringSize = j;
			}
		}
		Prediction = window[MaxStringPosition+1];		
	}

	// performance indicator: is the prediction good enough?
	return (MaxStringSize >= MinPerformance);
}
