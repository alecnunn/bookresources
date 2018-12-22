//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    AiBot.h
//----------------------------------------------------------------------------------------------

#ifndef __AIBOT_H
#define __AIBOT_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "paddle.h"
#include "tennispredictor.h"
#include "randompredictor.h"

//----------------------------------------------------------------------------------------------
// Defines constants
//----------------------------------------------------------------------------------------------

#define WINDOW_SIZE			10
#define MIN_PATTERN_SIZE	3
#define ANTICIPATION		1
#define TOWARDSCENTER		2
#define INTERPOLATION		3
#define TOWARDSBALL			4
#define PADDLE_ANGLES		9

//----------------------------------------------------------------------------------------------
// CAiBot: An AI-controlled tennis paddle
//----------------------------------------------------------------------------------------------

class CAiBot: public CPaddle, CObserver {
public:

	CAiBot() {
		pHorizontal = NULL;
		pVertical = NULL;
		RandomPredictor.Setup(PADDLE_ANGLES, GetTickCount());
	}

	~CAiBot() {
		if (pVertical != NULL) {
			delete pVertical;
		}
		if (pHorizontal != NULL) {
			delete pHorizontal;
		}
	}

public:
	void							SetupAI(CTennisBall * TennisBall, BOOL bTargeting = FALSE);
	void							EnableTargeting(BOOL bEnabled);
	virtual CTennisPredictor *		PredictorFactory();
	virtual BOOL					Notify(int msg, DWORD param = 0);
	virtual void					Paint(LPDDS lpdds);
	virtual void					Update();
	virtual void					ResetForService(BOOL HasService);
	virtual void					ShowStatistics(HDC dc);
	
protected:

	void							SeekTarget();
	void							AdvanceTarget();
	void							AdjustPaddleAngle();
	virtual void					OnBallCollision();
	void							PaintSequence(HDC dc, int x, int y, char * szA, char * szB,
										CTennisPredictor * pPredictor, int Width = 20);

protected:
	CTennisBall *					ball;
	CTennisPredictor *				pHorizontal;
	CTennisPredictor *				pVertical;
	
	int								nBounces;
	POINT							target;
	POINT							BallDestination;
	int								halfwidth, halfheight;
	int								heuristic;
	BOOL							bHasService;
	BOOL							bTargeting;
	CRandomPredictor				RandomPredictor;
};

//----------------------------------------------------------------------------------------------
#endif // __AIBOT_H

