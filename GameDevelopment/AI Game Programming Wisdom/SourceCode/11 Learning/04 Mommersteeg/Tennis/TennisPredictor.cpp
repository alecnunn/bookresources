//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    TennisPredictor.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "tennispredictor.h"

//----------------------------------------------------------------------------------------------
// GetWindowSize(): returns the size of the window
//----------------------------------------------------------------------------------------------

int CTennisPredictor::GetWindowSize() {
	return m_Window.GetSize();
}

//----------------------------------------------------------------------------------------------
// GetWindowEntry(): returns an entry in the window
//----------------------------------------------------------------------------------------------

int CTennisPredictor::GetWindowEntry(int i) {
	return m_Window[i].Element;
}

//----------------------------------------------------------------------------------------------
// GetMaxMatchSize(): returns the size of the longest substring that matches the tail of the
//					  window.
//----------------------------------------------------------------------------------------------

int CTennisPredictor::GetMaxMatchSize() {
	return m_nMaxSize;
}

//----------------------------------------------------------------------------------------------
// GetMaxMatchPosition(): returns the position (defined by its last element) of the longest 
//						  substring that matches the tail of the window.	
//----------------------------------------------------------------------------------------------

int CTennisPredictor::GetMaxMatchPosition() {
	return m_nMaxPosition;
}
