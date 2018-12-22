//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Paddle.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "paddle.h"
#include <math.h>

//----------------------------------------------------------------------------------------------
// Initialize static member
//----------------------------------------------------------------------------------------------

int CPaddle::instantiations = 0;

//----------------------------------------------------------------------------------------------
// ResetForService(): Resets the paddle for the service
//----------------------------------------------------------------------------------------------

void CPaddle::ResetForService(BOOL HasService) {
	if (direction == PADDLE_DOWN) {
		SetPosition((field.right+field.left) / 2, (2*field.top+field.bottom) / 3);
	} else {
		SetPosition((field.right+field.left) / 2, (field.top+2*field.bottom) / 3);
	}
}

//----------------------------------------------------------------------------------------------
// Left(): Move paddle to the left
//----------------------------------------------------------------------------------------------

void CPaddle::Left() {
	if (px>field.left) {
		px -= NormSpeed(STEP_SIZE);
	}
	RecalculatePaddle();
}

//----------------------------------------------------------------------------------------------
// Right(): Move paddle to the right
//----------------------------------------------------------------------------------------------
	
void CPaddle::Right() {
	if (px<field.right) {
		px += NormSpeed(STEP_SIZE);
	}
	RecalculatePaddle();
}

//----------------------------------------------------------------------------------------------
// Up(): Move paddle up
//----------------------------------------------------------------------------------------------

void CPaddle::Up() {
	if (py>field.top) {
		py -= NormSpeed(STEP_SIZE);
	}
	RecalculatePaddle();
}

//----------------------------------------------------------------------------------------------
// Down(): Move paddle down
//----------------------------------------------------------------------------------------------

void CPaddle::Down() {
	if (py<field.bottom) {
		py += NormSpeed(STEP_SIZE);
	}
	RecalculatePaddle();
}

//----------------------------------------------------------------------------------------------
// Rotate(): Rotate the paddle
//----------------------------------------------------------------------------------------------

void CPaddle::Rotate(int angle) {
	if (abs(rotation+angle)<MAX_ROTATION) {
		rotation += (int)NormSpeed(angle);
		nx = (float)cos((float)(rotation+90)/57.3);
		ny = (float)sin((float)(rotation+90)/57.3);
	}
	RecalculatePaddle();
}

//----------------------------------------------------------------------------------------------
// RecalculatePaddle(): Recalculate the shape of the paddle
//----------------------------------------------------------------------------------------------

void CPaddle::RecalculatePaddle() {
	paddle[0].x = (int)(px - (PADDLE_SIZE*cos((float)rotation/57.3)));
	paddle[0].y = (int)(py - (PADDLE_SIZE*sin((float)rotation/57.3)));
	paddle[1].x = (int)(px + (PADDLE_SIZE*cos((float)rotation/57.3)));
	paddle[1].y = (int)(py + (PADDLE_SIZE*sin((float)rotation/57.3)));
	paddle[2].x = paddle[1].x+(int)(PADDLE_WIDTH*nx);
	paddle[2].y = paddle[1].y+(int)(PADDLE_WIDTH*ny);
	paddle[3].x = paddle[0].x+(int)(PADDLE_WIDTH*nx);
	paddle[3].y = paddle[0].y+(int)(PADDLE_WIDTH*ny);

	if (region != NULL) {
		DeleteObject(region);
	}
	region = CreatePolygonRgn(paddle, 4, ALTERNATE);
}

//----------------------------------------------------------------------------------------------
// BallCollision(): Check for a collision between this paddle and the ball
//----------------------------------------------------------------------------------------------

void CPaddle::BallCollision(CTennisBall * ball) {
	RECT ballRect;
	ball->GetBoundingRect(&ballRect);
	if (ball->GetHeight()<PADDLE_HEIGHT) {
		if (RectInPaddle(&ballRect)) {
			ball->SetOwner(id);
			NotifyAll(HIT_BALL_EVENT, (DWORD)this);
			ball->BounceBall((float)direction*(HIT_FORCE*nx), (float)direction*(HIT_FORCE*ny));
			BOOL StillInPaddle = TRUE;
			while (StillInPaddle) {
				ball->Update();
				ball->GetBoundingRect(&ballRect);
				StillInPaddle = RectInPaddle(&ballRect);
			}
			// at the end of the collision the ball and paddle no longer collide
			OnBallCollision();
		}
	}
}

//----------------------------------------------------------------------------------------------
// PtInPaddle(): Checks if a point is inside the shape of the paddle
//----------------------------------------------------------------------------------------------

BOOL CPaddle::PtInPaddle(int x, int y) {
	return PtInRegion(region, x, y);
}

//----------------------------------------------------------------------------------------------
// RectInPaddle(): Checks if a rectangle is overlaps the shape of the paddle
//----------------------------------------------------------------------------------------------

BOOL CPaddle::RectInPaddle(const RECT * lpRect) {
	return RectInRegion(region, lpRect);
}

//----------------------------------------------------------------------------------------------
// Paint(): Paints the paddle
//----------------------------------------------------------------------------------------------

void CPaddle::Paint(LPDDS lpdds) {
		
	HDC dc;
	lpdds->GetDC(&dc);	

	HBRUSH brush = CreateSolidBrush(color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(dc, brush);
	HPEN pen = CreatePen(PS_NULL, 0, 0);
	HPEN oldPen = (HPEN)SelectObject(dc, pen);
	
	SetPolyFillMode(dc, ALTERNATE);
	Polygon(dc, paddle, 4);
	
	if (oldPen != NULL) {
		SelectObject(dc, oldPen);
		DeleteObject(brush);
	}
	if (oldBrush != NULL) {
		SelectObject(dc, oldBrush);
		DeleteObject(pen);
	}

	lpdds->ReleaseDC(dc);
}
