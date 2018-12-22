//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Predictor.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
//	The building block for a predictor is CPredictor. It offers three abstract methods that 
//	should be overriden by derived classes:
//
//	virtual void Update(int NextElement);	
//	Call this method to update the predictor with the next element of the sequence. 
//	
//	virtual bool Prediction(int &Prediction);
//	Call this method to get a reasonable successor for the sequence. If the return value is
//	false, the prediction is bad and should not be used.
//
//	virtual void Reset();
//	Call this method to reset the predictor (i.e. lose its memory)
//----------------------------------------------------------------------------------------------

#ifndef __PREDICTOR_H
#define __PREDICTOR_H

//----------------------------------------------------------------------------------------------
// CPredictor: A predictor tries to predict the next element in a series of numbers (abstract)
//----------------------------------------------------------------------------------------------

class CPredictor {
public:
	// abstract interface methods
	virtual void	Update(int NextElement) = 0;
	virtual bool	GetPrediction(int &Prediction) = 0;
	virtual void	Reset() = 0;
};

//----------------------------------------------------------------------------------------------
#endif // __PREDICTOR_H


