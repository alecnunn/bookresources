//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    SelfAdjustingPredictor.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "selfadjustingpredictor.h"

//----------------------------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------------------------

const float g_Punishment = 1.1f;
const float g_Reward = 0.9f;

//----------------------------------------------------------------------------------------------
// Update(): overloaded to evaluate previous prediction
//----------------------------------------------------------------------------------------------

void CSelfAdjustingPredictor::Update(int NextElement) {
	
	// did the predictor think the estimation was correct?
	if (m_EstimatedCorrect) {
		// check if the predictor was wrong
		if (NextElement != m_nPrediction) {
			// punish the predictor
			m_fMinPerformance *= g_Punishment;
		} else {
			// reward the predictor if the prediction was correct
			if (NextElement==m_nPrediction) {
				m_fMinPerformance *= g_Reward;
			}
		}
	}
	

	// add next element to the predictor
	CImprovedPredictor::Update(NextElement);
}

//----------------------------------------------------------------------------------------------
// GetPrediction(): overloaded to catch the correctness estimation
//----------------------------------------------------------------------------------------------

bool CSelfAdjustingPredictor::GetPrediction(int &Prediction) {

	// catch the correctness estimation (for evaluation during next update)
	m_EstimatedCorrect = CImprovedPredictor::GetPrediction(Prediction);
	return m_EstimatedCorrect;
}