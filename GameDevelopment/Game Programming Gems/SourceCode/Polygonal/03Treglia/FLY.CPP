/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
//===========================================================================
//===========================================================================

#ifdef _WIN32
#include <windows.h>
#endif
#include <cstring>
#include <cmath>
#include "fly.h"

#define PI 3.14159265359F
#define PI_2 6.28318530718F
#define PI_OVER_2 1.57079632679F
#define RAD_TO_DEG(rad) (57.2957795131F * rad)

//-------------------------------------------------------------------------
// An asymptotic effect returns a float that approaches targetX from currX 
//-------------------------------------------------------------------------
float Damp(float currX, float targetX) {
	return currX + ((targetX - currX) / 16.0F);
}

//---------------------------------------------------------------------------
// Spring Damp Function
//---------------------------------------------------------------------------
vector3 SpringDamp(
		vector3 currPos,    // Current Position
		vector3 trgPos,     // Target Position
		vector3 prevTrgPos, // Previous Target Position
		float deltaTime,    // Change in Time
		float springConst,  // Hooke's Constant
		float dampConst,    // Damp Constant
		float springLen) {
	vector3 disp;           // Displacement
	vector3 velocity;       // Velocity   
	float forceMag;         // Force Magnitude

	// Calculate Spring Force
	disp = currPos - trgPos;
	velocity = (prevTrgPos - trgPos) * deltaTime;
	forceMag = springConst * (springLen - disp.length()) + 
		dampConst * (DotProduct(disp, velocity) / disp.length());

	// Apply Spring Force
	disp.normalize();
	disp *= forceMag * deltaTime;
	return currPos += disp;
}

//-------------------------------------------------------------------------
// Fly Cam Constructor
//-------------------------------------------------------------------------
FlyCam::FlyCam(vector3 & eyeIn, float yIn, float pIn, float rIn) {
	// Flying Cam
	eye = eyeIn;
	Yaw = yIn;
	Pitch = pIn;
	Roll = rIn;

	// Init
	fwdS = 100;
	eyeTrg = eyeIn + vector3(0.0, 0.0, -1.0);
	eyeTrgPrev = eyeIn + vector3(0.0, 0.0, -1.0);
	angTrg.set(0.0, 0.0, 0.0);
	angTrgPrev.set(0.0, 0.0, 1.0);
}

//-------------------------------------------------------------------------
// Compute Parameters
//-------------------------------------------------------------------------
void FlyCam::ComputeInfo() {
	float cosY, cosP, cosR;
	float sinY, sinP, sinR;

	// Only Want to Calc these once
	cosY = cosf(Yaw);
	cosP = cosf(Pitch);
	cosR = cosf(Roll);
	sinY = sinf(Yaw);
	sinP = sinf(Pitch);
	sinR = sinf(Roll);

	// Fwd Vector
	fwd.x = sinY * cosP;
	fwd.y = sinP;
	fwd.z = cosP * -cosY;

	// Look At Point
	at = fwd + eye;

	// Up Vector
	up.x = -cosY * sinR - sinY * sinP * cosR;
	up.y = cosP * cosR;
	up.z = -sinY * sinR - sinP * cosR * -cosY;

	// Side Vector (right)
	side = CrossProduct(fwd, up);
}

//-------------------------------------------------------------------------
// Compute viewing matrix according to flight
//-------------------------------------------------------------------------
matrix44 FlyCam::GetViewMtx() {
	// Compute Current Info
	ComputeInfo();

	// View matrix
	return LookAtMatrix44(eye, at, up);
}

//-------------------------------------------------------------------------
// Set Yaw and Pitch with mouse
//-------------------------------------------------------------------------
void FlyCam::Mouse(float x, float y) {
	float YT, PT;

	YT = RANGE(x, 0.0F, 1.0F, -3.1415F, 3.1415F);
	PT = RANGE(y, 0.0F, 1.0F, 1.5707F, -1.5707F);

	angTrg.set(YT, PT, 0.0);
}

//-------------------------------------------------------------------------
// Set Flags with keyboard data
//-------------------------------------------------------------------------
void FlyCam::Tick(float deltaSecs) {
	// Damp Angles
	vector3 currAng = SpringDamp(vector3(Yaw, Pitch, 0.0), angTrg, angTrgPrev, deltaSecs, 4.5F, 3.5F, 0.0001F);
	Yaw = currAng.x;
	Pitch = currAng.y;
	angTrgPrev = angTrg;

	// Store Info
	eyeTrgPrev = eyeTrg;

	// Keyboard Data
	if (GetKeyState('W') & 0x80) eyeTrg += deltaSecs * fwdS * fwd;
	if (GetKeyState('S') & 0x80) eyeTrg -= deltaSecs * fwdS * fwd;
	if (GetKeyState('D') & 0x80) eyeTrg += deltaSecs * fwdS * side;
	if (GetKeyState('A') & 0x80) eyeTrg -= deltaSecs * fwdS * side;
	if (GetKeyState(VK_UP) & 0x80) eyeTrg += deltaSecs * fwdS * fwd;
	if (GetKeyState(VK_DOWN) & 0x80) eyeTrg -= deltaSecs * fwdS * fwd;
	if (GetKeyState(VK_RIGHT) & 0x80) eyeTrg += deltaSecs * fwdS * side;
	if (GetKeyState(VK_LEFT) & 0x80) eyeTrg -= deltaSecs * fwdS * side;
	if (GetKeyState('R') & 0x80) eyeTrg += deltaSecs * fwdS * up;
	if (GetKeyState('F') & 0x80) eyeTrg -= deltaSecs * fwdS * up;

	eye = SpringDamp(eye, eyeTrg, eyeTrgPrev, deltaSecs, 2.5F, 0.5F, 1.0F);

	// Reset Cursor If needed
	POINT p;
	RECT r;
	GetCursorPos(&p);
	GetWindowRect(GetActiveWindow(), &r);
	if (!PtInRect(&r, p)) {
		int x = p.x;

		if (p.x < r.left) {
			x = r.right - (r.left - p.x);
			Yaw += 2 * PI;
		}
		if (p.x > r.right) {
			x = r.left + (p.x - r.right);
			Yaw -= 2 * PI;
		}
		SetCursorPos(x, p.y);
	}
}
