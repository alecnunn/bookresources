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
#ifndef _FLY_H
#define _FLY_H
#include "mtxlib.h"
#include "quaternion.h"

#define UNITS_PER_SEC 40.0F
#define FLYSetUnitsPerFrame(framesPerSecond) \
	(flyCam.unitsPerFrame = UNITS_PER_SEC / framesPerSecond)

class FlyCam {
public:
	// FlyCam(vector3 & eyeIn, vector3 & atIn, vector3 & upIn);
	FlyCam(vector3 & eyeIn, float aIn, float eIn, float rIn);

private:
	// Position (Point)
	vector3 eye;
	// Forward (Point) (IE LookAt)
	vector3 at;
	// Up (Vector)
	vector3 up;
	// Forward (Vector) (IE View Direction)
	vector3 fwd;
	// Side (right)
	vector3 side;

	// Orientation data
	// Yaw, Pitch, and Roll
	float Yaw, Pitch, Roll;
	vector3 angTrg;
	vector3 angTrgPrev;

	// Forward Speed
	float fwdS;

	// Damp Targets
	vector3 eyeTrg;
	vector3 eyeTrgPrev;

public:
	void ComputeInfo();
	matrix44 GetViewMtx();
	void Tick(float deltaTime);
	void Mouse(float x, float y);
	void Keyboard();
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
float Damp(float currX, float targetX);
vector3 SpringDamp(vector3 currPos, vector3 trgPos, vector3 prevTrgPos, 
		float deltaTime, float springConst, float dampConst, float springLen);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define RANGE(x, minX, maxX, minY, maxY) \
	((((x - minX) * (maxY - minY))/(maxX - minX)) + minY)

#endif 
