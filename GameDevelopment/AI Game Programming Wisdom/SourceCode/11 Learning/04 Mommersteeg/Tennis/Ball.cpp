//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Ball.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ball.h"
#include <math.h>

//----------------------------------------------------------------------------------------------
// InterpolateToBouncePosition(): interpolates the ball to where it will hot the ground
//----------------------------------------------------------------------------------------------

void CTennisBall::InterpolateToBouncePosition(POINT &pt) {
	float D = vz*vz+2*pz*GROUND_ACCELERATION;
	if (D>=0) {
		float t = (vz-(float)sqrt(D)) / (-GROUND_ACCELERATION);
		t += 20; // let the ball bounce first
		pt.x = (int)(px+vx*t);
		pt.y = (int)(py+vy*t);
	}
}

//----------------------------------------------------------------------------------------------
// BounceBall(): bounces the ball with a paddle
//----------------------------------------------------------------------------------------------

void CTennisBall::BounceBall(float x, float y) {
	SetVelocity(x, y, UPWARD_BOUNCE_FORCE);
	InterpolateToBouncePosition(BouncePosition);
}

//----------------------------------------------------------------------------------------------
// Update(): Updates the ball state
//----------------------------------------------------------------------------------------------

void CTennisBall::Update() {	
	ApplyGroundForce();
	ApplyVelocity();
}

//----------------------------------------------------------------------------------------------
// GetBoundingRect(): Returns a bounding rectangle for the ball
//----------------------------------------------------------------------------------------------

void CTennisBall::GetBoundingRect(RECT * lpRect) {
	lpRect->left = (int)px-BALL_SIZE;
	lpRect->right = (int)px+BALL_SIZE;
	lpRect->top = (int)py-BALL_SIZE;
	lpRect->bottom = (int)py+BALL_SIZE;
}

//----------------------------------------------------------------------------------------------
// ApplyGroundForce(): Applies ground force to the ball
//----------------------------------------------------------------------------------------------

void CTennisBall::ApplyGroundForce() {
	if (pz > 0) {
		vz = vz + NormSpeed(GROUND_ACCELERATION);	
	}
}

//----------------------------------------------------------------------------------------------
// ApplyVelocity(): Applies the velocity vector to the ball
//----------------------------------------------------------------------------------------------

void CTennisBall::ApplyVelocity() {
	pz = pz - NormSpeed(vz);

	if (pz < 0 && owner != OWNER_SERVICE) {
		NotifyAll(HIT_GROUND_EVENT);
	}
	
	if (pz < 0) {
		vz = - (vz-GROUND_FRICTION);
		pz = (float)abs((int)vz);
		if (pz<GROUND_ACCELERATION) {
			pz = 0;
			vz = 0;
		}		
	}

	if (pz == 0) {
		NotifyAll(BALL_ROLLING_EVENT);
	}
	
	px = px + NormSpeed(vx);
	py = py + NormSpeed(vy);
}

//----------------------------------------------------------------------------------------------
// Paint(): Paint the ball
//----------------------------------------------------------------------------------------------

void CTennisBall::Paint(LPDDS lpdds) {
	HDC dc;
	lpdds->GetDC(&dc);
	int scale = BALL_SIZE + (int)(pz/2);

	HBRUSH brush = CreateSolidBrush(RGB(255,255,0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(dc, brush);	
	HPEN pen = CreatePen(PS_NULL, 0, 0);
	HPEN oldPen = (HPEN)SelectObject(dc, pen);

	Ellipse(dc, (int)px-scale, (int)py-scale, (int)px+scale, (int)py+scale);

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
