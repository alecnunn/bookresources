//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    ImprovedPredictor.h
//----------------------------------------------------------------------------------------------

#ifndef __IMPROVEDPREDICTOR_H
#define __IMPROVEDPREDICTOR_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "StringMatchPredictor.h"
#include "Array.h"

//----------------------------------------------------------------------------------------------
// CImprovedPredictor: Enhances the standard string-matching predictor by taking all matching
//					   substrings into account
//
// Note: The Performance Indicator is normalized. This means that when you set up the predictor,
// the fMinPerformance parameter is a chance (between 0 and 1) that indicates the minimum
// desired probability that the prediction is correct.
//----------------------------------------------------------------------------------------------

class CImprovedPredictor : public CStringMatchPredictor {
public:
	void			Setup(int nWindowSize, int nAlphabetSize, float fMinPerformance);
	virtual void	Update(int NextElement);
	virtual bool	GetPrediction(int &Prediction);

protected:
	void			CalculatePrediction();

protected:
	CArray <float>	m_Alphabet;
	float			m_TotalPredictionValue;
	float			m_HighestPredictionValue;
	float			m_fMinPerformance;
};

//----------------------------------------------------------------------------------------------
#endif // __IMPROVEDPREDICTOR_H
