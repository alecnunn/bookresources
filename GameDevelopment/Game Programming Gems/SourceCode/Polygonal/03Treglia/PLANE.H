/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef _PLANE_H
#define _PLANE_H
#include "object.h"
#include <cstdio>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------
class Plane : public Object {
public:
	Plane(float tileXSize, float tileYSize, int tileXNum, 
		int tileYNum, int texXNum, int texYNum, COLOR color);
	void DrawPrimitives();

public:
	// W x L of tile + Number of tiles in each direction
	float tileXSize;
	int tileXNum;
	float xSize;
	float tileYSize;
	int tileYNum;
	int ySize;

	COLOR color;
	vector3 normal;

public:
	void Randomize();
};
#endif
