//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    RandomPredictor.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "randompredictor.h"

//----------------------------------------------------------------------------------------------
// Setup(): sets up the random predictor (seeds the random number generator)
//----------------------------------------------------------------------------------------------

void CRandomPredictor::Setup(int nAlphabetSize, unsigned int nSeed) {
	m_nAlphabetSize = nAlphabetSize;
	srand(nSeed);
}

//----------------------------------------------------------------------------------------------
// GetPrediction(): returns a random prediction
//----------------------------------------------------------------------------------------------

bool CRandomPredictor::GetPrediction(int &Prediction) {

	Prediction = rand() % m_nAlphabetSize;

	// always assume that the prediction is good
	return true;
}
