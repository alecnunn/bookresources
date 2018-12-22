//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    ImprovedPredictor.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "ImprovedPredictor.h"

//----------------------------------------------------------------------------------------------
// Update(): updates the predictor with the next element in the sequence
//----------------------------------------------------------------------------------------------

void CImprovedPredictor::Update(int NextElement) {

	// calculate lenghts of all substrings matching the tail
	int nSize = m_Histogram[NextElement].GetSize();

	// clear alphabet array
	for (int n = 0; n < m_Alphabet.GetSize(); n++) {
		m_Alphabet[n] = 0.0f;
	}
	
	// clear total prediction value;
	m_TotalPredictionValue = 0.0f;

	int i = 0;
	// find all occurrences of NextElement in the window
	while (i < nSize) {

		// retrieve the i-th occurence of NextElement
		THistogramData & hd = m_Histogram[NextElement][i];
		
		// determine the distance of this substring to the tail
		int DistanceToTail = (m_nSequenceLength - hd.nSequencePosition);
		
		// determine the position of this occurrence in the window
		int iWindowPosition = m_Window.GetSize() - DistanceToTail;

		if (HasNeighbour(iWindowPosition, m_PrevElement)) {
			// if this occurrence is preceded by PrevElement, we create a longer substring
			m_Window[iWindowPosition].nMatchSize = GetNeighbourSize(iWindowPosition) + 1;
		} else {
			// else the size of this substring has to be one
			m_Window[iWindowPosition].nMatchSize = 1;
		}

		// calculate the prediction value of this substring
		float PredictionValue = (float)(m_Window[iWindowPosition].nMatchSize) / (float)DistanceToTail;
	
		int Successor;

		// get the successor element of this matching substring
		if (iWindowPosition == m_Window.GetSize()-1) {
			Successor = NextElement;
		} else {
			Successor = m_Window[iWindowPosition + 1].Element;
		}

		// add this prediction value to this element
		m_Alphabet[Successor] += PredictionValue;

		// update total prediction value (used to normalize the predictions later on)
		m_TotalPredictionValue += PredictionValue;

		i++;
	}

	// add NextElement to the histogram 
	m_Histogram[NextElement].AddHead(THistogramData(m_nSequenceLength));

	// remove element dropped from the window also from histogram
	TSequenceData * pDropped = m_Window.Add(TSequenceData(NextElement));
	if (pDropped != NULL) {		
		m_Histogram[pDropped->Element].RemoveTail();
	}

	// update predictor/sequence information
	m_nSequenceLength++;
	m_PrevElement = NextElement;

	// calculate predicted successor
	CalculatePrediction();
}

//----------------------------------------------------------------------------------------------
// CalculatePrediction(): calculates the best possible prediction
//----------------------------------------------------------------------------------------------

void CImprovedPredictor::CalculatePrediction() {
	int i;

	if (m_TotalPredictionValue > 0) {

		// normalize prediction values
		for (i = 0; i < m_Alphabet.GetSize(); i++) {
			m_Alphabet[i] /= m_TotalPredictionValue;
		}

		// find element with the highest prediction value
		m_HighestPredictionValue = 0.0f;	
		for (i = 0; i < m_Alphabet.GetSize(); i++) {
			if (m_Alphabet[i] > m_HighestPredictionValue) {
				m_HighestPredictionValue = m_Alphabet[i];
				m_nPrediction = i;
			}
		}

	} else {
		m_nPrediction = 0;
	}
}

//----------------------------------------------------------------------------------------------
// GetPrediction(): returns the best possible prediction
//----------------------------------------------------------------------------------------------

bool CImprovedPredictor::GetPrediction(int &Prediction) {
	// return cached prediction value
	Prediction = m_nPrediction;
	return m_HighestPredictionValue >= m_fMinPerformance;
}

//----------------------------------------------------------------------------------------------
// Setup(): sets up the predictor
//----------------------------------------------------------------------------------------------

void CImprovedPredictor::Setup(int nWindowSize, int nAlphabetSize, float fMinPerformance) {
	// set up window
	m_nWindowSize = nWindowSize;
	m_Window.SetSize(nWindowSize);

	// set up histogram
	m_nAlphabetSize = nAlphabetSize;
	m_Histogram.SetSize(nAlphabetSize);
	m_Alphabet.SetSize(nAlphabetSize);

	m_fMinPerformance = fMinPerformance;	
}
