//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    PatternBot.h
//----------------------------------------------------------------------------------------------

#ifndef __PATTERNBOT_H
#define __PATTERNBOT_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "aibot.h"
#include "array.h"

//----------------------------------------------------------------------------------------------
// CPatternBot: An AI-controlled bot that can generate patterns in where it plays the ball to
//----------------------------------------------------------------------------------------------

class CPatternBot : public CAiBot {
public:

	void				SetPatternSize(int nSize);
	int 				GetPatternSize();

protected:
	
	virtual void		ShowStatistics(HDC dc) { /* do not show statistics */ }
	virtual void		OnBallCollision();
	virtual void		ResetForService(BOOL HasService);

protected:

	CArray <int>		m_Pattern;
	int					m_PatternPos;

};

//----------------------------------------------------------------------------------------------
#endif // __PATTERNBOT_H
