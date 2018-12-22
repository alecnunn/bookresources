//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    TennisField.h
//----------------------------------------------------------------------------------------------

#ifndef __TENNISFIELD_H
#define __TENNISFIELD_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "paddle.h"
#include "ball.h"
#include "observer.h"

//----------------------------------------------------------------------------------------------
// Defined constants
//----------------------------------------------------------------------------------------------

#define PLAYER1    0
#define PLAYER2    1
#define NET_WIDTH  5
#define NET_HEIGHT 0 // disable net

//----------------------------------------------------------------------------------------------
// CTennisField: Manages the game objects
//----------------------------------------------------------------------------------------------

class CTennisField : public CObserver {

public:

	void			SetupField(LPRECT pField, CPaddle * Player1, CPaddle * Player2, CTennisBall * Ball);
	void			Update();
	BOOL			Notify(int msg, DWORD param);
	void			SetupService(int ServicePlayer);
	void			NetCollision();
	void			Start() { StartTime = timeGetTime()-1000; nFrameCount = DEFAULT_SPEED; }

protected:
	BOOL			BallInField(int index, POINT pt);
	void			PlayerScores(int index);
	void			ShowPlayerScore(HDC dc, int x, int y, int PlayerId);
	void			ShowFPS(HDC dc, int x, int y);

protected:

	HWND			hWnd;
	RECT			field;
	CPaddle *		player[2];	
	CTennisBall *	ball;
	DWORD			StartTime;
	DWORD			nFrameCount;
};

//----------------------------------------------------------------------------------------------
#endif // __TENNISFIELD_H
