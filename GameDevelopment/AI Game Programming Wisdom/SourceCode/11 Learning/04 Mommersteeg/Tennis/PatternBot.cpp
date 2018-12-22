//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    PatternBot.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "patternbot.h"

//----------------------------------------------------------------------------------------------
// SetPatternSize(): Sets the size of the pattern that is generated
//----------------------------------------------------------------------------------------------

void CPatternBot::SetPatternSize(int nSize) {
	m_Pattern.SetSize(nSize);

	for (int i = 0; i<m_Pattern.GetSize(); i++) {
		int Destination;

		// randomly chooce a destination to play the ball to
		RandomPredictor.GetPrediction(Destination);

		// calculate location
		m_Pattern[i] = field.left + OUTBOX_WIDTH + (Destination * (field.right-field.left-2*OUTBOX_WIDTH)) / PADDLE_ANGLES;
	}

	// reset positition in the pattern
	m_PatternPos = 0;

	// enable targeting
	bTargeting = TRUE;

	// set first target
	OnBallCollision();
}

//----------------------------------------------------------------------------------------------
// OnBallCollision(): Determine next ball destination (to maintain the pattern)
//----------------------------------------------------------------------------------------------

void CPatternBot::OnBallCollision() {
	BallDestination.x = m_Pattern[m_PatternPos];
	BallDestination.y = (int)py + direction * (field.bottom-field.top);

	m_PatternPos++;
	if (m_PatternPos >= m_Pattern.GetSize()) {
		m_PatternPos = 0;
	}
}

//----------------------------------------------------------------------------------------------
// ResetForService(): resets the pattern bot for the service, and generates a new pattern
//----------------------------------------------------------------------------------------------

void CPatternBot::ResetForService(BOOL HasService) {
	// call default parent implementation
	CAiBot::ResetForService(HasService);

	// generate a new pattern
	SetPatternSize(m_Pattern.GetSize());
}
