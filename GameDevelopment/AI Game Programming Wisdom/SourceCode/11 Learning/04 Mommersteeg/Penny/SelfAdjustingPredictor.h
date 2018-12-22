//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    SelfAdjustingPredictor.h
//----------------------------------------------------------------------------------------------

#ifndef __SELFADJUSTINGPREDICTOR_H
#define __SELFADJUSTINGPREDICTOR_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "improvedpredictor.h"

//----------------------------------------------------------------------------------------------
// CSelfAdjustingPredictor: automatically adjusts the performance indicator
//----------------------------------------------------------------------------------------------

class CSelfAdjustingPredictor : public CImprovedPredictor {
public:

	CSelfAdjustingPredictor() { m_EstimatedCorrect = false; }

	virtual void	Update(int NextElement);
	virtual bool	GetPrediction(int &Prediction);

protected:

	bool			m_EstimatedCorrect;

};

//----------------------------------------------------------------------------------------------
#endif // __SELFADJUSTINGPREDICTOR_H