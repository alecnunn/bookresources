//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    RandomPredictor.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
//	The random predictor is just a random number generator for a specified domain.
//	It uses the standard C function rand() to generate a pseudo random number.
//----------------------------------------------------------------------------------------------

#ifndef __RANDOMPREDICTOR_H
#define __RANDOMPREDICTOR_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "predictor.h"

//----------------------------------------------------------------------------------------------
// CRandomPredictor: Randomly predicts the next element in the sequence
//----------------------------------------------------------------------------------------------

class CRandomPredictor : public CPredictor {
public:

	// implementation of interface CPredictor
	virtual void	Update(int NextElement) { /* just ignore updates */ }
	virtual bool	GetPrediction(int &Prediction);
	virtual void	Reset() { /* just ignore resets */ }

	// setup method
	virtual void	Setup(int nAlphabetSize, unsigned int nSeed = 0);

protected:
	unsigned int	m_nAlphabetSize;

};

//----------------------------------------------------------------------------------------------
#endif // __RANDOMPREDICTOR_H
