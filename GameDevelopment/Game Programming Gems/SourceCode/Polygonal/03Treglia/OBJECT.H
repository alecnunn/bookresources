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
#ifndef _OBJECT_H
#define _OBJECT_H
#include "mtxlib.h"
#include "def.h"
#include <cassert>
#include <cstdio>

//---------------------------------------------------------------------------
// The Object Base Class
//---------------------------------------------------------------------------
class Object {
public:
	Object();
	virtual ~Object();

// Draw
public:
	int vtxCnt;
	void * vtxPtr;
	void * nrmPtr;
	void * clrPtr;
	void * txcPtr;

public:
	virtual void DrawPrimitives() = 0;
	virtual void Draw(matrix44 &viewMtx);

// Orientation
public:
	vector3 position;
	matrix44 orientation;
	matrix44 modelView;
	matrix44 normalMtx;

public:
	void LoadPositionMatrix();

// Lighting
public:
	int lightsOn;
	COLOR ambient;
	COLOR material;
	void * lights[8];
	int numLights;

public:
	void EnableLights();
	void DisableLights();
	void AddLight(void * light);
	void SetMaterial(COLOR ambient, COLOR material);
	void SetMaterial(float ambient, COLOR material);
	void LoadNormalMatrix();
};
#endif
