//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Paddle.h
//----------------------------------------------------------------------------------------------


#ifndef __PADDLE_H
#define __PADDLE_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "directdraw.h"
#include "observer.h"
#include "ball.h"

//----------------------------------------------------------------------------------------------
// Defined constants
//----------------------------------------------------------------------------------------------

#define MAX_ROTATION  50
#define PADDLE_SIZE	  30
#define PADDLE_WIDTH  10
#define STEP_SIZE	  1.9f
#define HIT_FORCE	  5
#define PADDLE_HEIGHT 30
#define PADDLE_UP	  -1
#define PADDLE_DOWN   1
#define ROTATE_SPEED  2

//----------------------------------------------------------------------------------------------
// CPaddle: Represents a paddle (which can be controlled by a player to return the ball)
//----------------------------------------------------------------------------------------------

class CPaddle : public CSubject {

public:
					CPaddle() {
						region = NULL;
						instantiations++;
						id = instantiations;						
					}

					~CPaddle() {
						if (region != NULL) {
							DeleteObject(region);
						}
					}

public:
	virtual void	ResetForService(BOOL HasService);
	void			SetPosition(int x, int y, int angle = 0) { px = (float)x; py = (float)y; rotation = 0; Rotate(angle); }
	void			SetPlayField(LPRECT lpRect) { field = *lpRect; }
	void			SetColor(COLORREF ref) { color = ref; }
	void			SetDirection(int dir) { direction = dir; } 
	BOOL			PtInPaddle(int x, int y);
	BOOL			RectInPaddle(const RECT * lpRect);
	void			BallCollision(CTennisBall * ball);
	LPRECT			GetField() { return &field; }
	int				GetId() { return id; }
	int				GetDirection() { return direction; }
	int				GetColor() { return color; }

	void			Left(); 
	void			Right(); 
	void			Up(); 
	void			Down(); 
	
	virtual void	Paint(LPDDS lpdds);
	void			Rotate(int angle);

	virtual void	Update() { /* skip */ }

public:
	int				bounce; // used by TennisField - do not use in CPaddle or derived classes
	int				score;  // used by TennisField - do not use in CPaddle or derived classes

protected:
	void			RecalculatePaddle();
	virtual void	OnBallCollision() { /* empty default implementation */ }

protected:
	static int		instantiations;
	int				id;
	POINT			paddle[4];
	HRGN			region;
	COLORREF		color;
	RECT			field;
	float			px, py;
	int				rotation;
	float			nx, ny;
	int				direction;
};

//----------------------------------------------------------------------------------------------
#endif // __PADDLE_H
