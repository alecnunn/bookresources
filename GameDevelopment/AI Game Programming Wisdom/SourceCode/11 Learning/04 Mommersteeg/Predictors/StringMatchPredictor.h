//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    StringMatchPredictor.h
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// An efficient O(N) string-matching predictor (where N is the size of the window).
//----------------------------------------------------------------------------------------------

#ifndef __STRINGMATCHPREDICTOR_H
#define __STRINGMATCHPREDICTOR_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "slidingwindow.h"
#include "iteratorlist.h"
#include "array.h"
#include "predictor.h"

//----------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------

#define SMP_HISTOGRAM CArray< CIteratorList< THistogramData > >
#define SMP_WINDOW CSlidingWindow <TSequenceData>

//----------------------------------------------------------------------------------------------
// THistorgramData: element in the histogram
//----------------------------------------------------------------------------------------------

struct THistogramData {
public:
	THistogramData() {}
	THistogramData(int nSequencePosition) { this->nSequencePosition = nSequencePosition; }
public:
	int	nSequencePosition;
};

//----------------------------------------------------------------------------------------------
// TSequenceData: element in the sequence
//----------------------------------------------------------------------------------------------

struct TSequenceData {
public:
	TSequenceData() {}
	TSequenceData(int Element) { this->Element = Element; }
public:
	int nMatchSize;
	int	Element;
};

//----------------------------------------------------------------------------------------------
// CStringMatchPredictor: efficient O^N string-matching predictor
//----------------------------------------------------------------------------------------------

class CStringMatchPredictor : public CPredictor {
public:
	// constructors & destructors

	CStringMatchPredictor() { m_nMaxSize = 0; m_nPrediction = 0; m_nSequenceLength = 0; m_nMinPerformance = 1; }
	~CStringMatchPredictor() {}

public:
	// override members from predictor interface
	virtual void	Update(int NextElement);
	virtual bool	GetPrediction(int &Prediction);
	virtual void	Reset();

public:
	// predictor setup method
	void			Setup(int nWindowSize, int nAlphabetSize, int nMinPerformance);

protected:
	BOOL			HasNeighbour(int iWindowPosition, int Neighbour);
	int				GetNeighbourSize(int iWindowPosition);

protected:
	SMP_HISTOGRAM	m_Histogram;
	SMP_WINDOW		m_Window;
	int				m_PrevElement;
	int				m_nWindowSize;
	int				m_nSequenceLength;
	int				m_nMaxSize;
	int				m_nMaxPosition;
	int				m_nPrediction;
	int				m_nMinPerformance;
	int				m_nAlphabetSize;
};

//----------------------------------------------------------------------------------------------
#endif // __STRINGMATCHPREDICTOR_H
