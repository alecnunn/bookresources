//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    StringMatchPredictor.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "StringMatchPredictor.h"
#include <assert.h>

//----------------------------------------------------------------------------------------------
// Setup(): sets up the predictor
//----------------------------------------------------------------------------------------------

void CStringMatchPredictor::Setup(int nWindowSize, int nAlphabetSize, int nMinPerformance) {
	// set up window
	m_nWindowSize = nWindowSize;
	m_Window.SetSize(nWindowSize);

	// set up histogram
	m_nAlphabetSize = nAlphabetSize;
	m_Histogram.SetSize(nAlphabetSize);

	m_nMinPerformance = nMinPerformance;	
}

//----------------------------------------------------------------------------------------------
// GetPrediction(): determines the prediction using string-matching prediction
//----------------------------------------------------------------------------------------------

bool CStringMatchPredictor::GetPrediction(int &Prediction) {
	// return cached prediction value
	Prediction = m_nPrediction;
	return m_nMaxSize >= m_nMinPerformance;
}

//----------------------------------------------------------------------------------------------
// Reset(): resets the predictor
//----------------------------------------------------------------------------------------------

void CStringMatchPredictor::Reset() {	
	m_nMaxSize = 0; 
	m_nPrediction = 0;
	m_nSequenceLength = 0;

	// clear window
	m_Window.SetSize(m_nWindowSize);

	// clear histogram
	for (int i=0; i<m_nAlphabetSize; i++) {
		m_Histogram[i].RemoveAll();
	}
}

//----------------------------------------------------------------------------------------------
// HasNeighbour(): checks if the given window entry has the specified left-hand neighbour
//----------------------------------------------------------------------------------------------

BOOL CStringMatchPredictor::HasNeighbour(int iWindowPosition, int Neighbour) {
	// is element at iWindowPosition preceded by Neighbour?
	if (iWindowPosition > 0) {
		if (m_Window[iWindowPosition - 1].Element == Neighbour) {
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------------
// NeighbourSize(): determines the match size of the left-hand neighbour of the specified 
//					window entry (which is only valid in special situations)
//----------------------------------------------------------------------------------------------

int	CStringMatchPredictor::GetNeighbourSize(int iWindowPosition) {
	// calcalute the size of the substring for the lower-index neighbour	
	return m_Window[iWindowPosition-1].nMatchSize;
}

//----------------------------------------------------------------------------------------------
// Update(): updates the predictor with the next element in the sequence
//----------------------------------------------------------------------------------------------

void CStringMatchPredictor::Update(int NextElement) {

	// calculate lenghts of all substrings matching the tail
	int nSize = m_Histogram[NextElement].GetSize();
	int nMaxLength = 0, nMaxPosition;
	int i = 0;

	// find all occurrences of NextElement in the window
	while (i < nSize) {

		// retrieve the i-th occurence of NextElement
		THistogramData & hd = m_Histogram[NextElement][i];
		
		// determine the position of this occurrence in the window
		int iWindowPosition = m_Window.GetSize() - (m_nSequenceLength - hd.nSequencePosition);

		if (HasNeighbour(iWindowPosition, m_PrevElement)) {
			// if this occurrence is preceded by PrevElement, we create a longer substring
			m_Window[iWindowPosition].nMatchSize = GetNeighbourSize(iWindowPosition) + 1;
		} else {
			// else the size of this substring has to be one
			m_Window[iWindowPosition].nMatchSize = 1;
		}

		// keep track of the length and position of the longest substring encountered so far
		if (m_Window[iWindowPosition].nMatchSize > nMaxLength) {
			nMaxLength = m_Window[iWindowPosition].nMatchSize;
			nMaxPosition = hd.nSequencePosition;
		}
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
	m_nMaxSize = nMaxLength;
	m_nMaxPosition = m_Window.GetSize() - (m_nSequenceLength - nMaxPosition);

	// calculate predicted successor (if a substring was found)
	if (nMaxLength > 0) {
		int nSuccessorPosition = m_nMaxPosition + 1;
		m_nPrediction = m_Window[nSuccessorPosition].Element;
	}
}
