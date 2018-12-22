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
#ifndef _LIGHT_H
#define _LIGHT_H
#include "light.h"
#include "object.h"
#include "mtxlib.h"
#include "def.h"

//---------------------------------------------------------------------------
// The Light Base Class
//---------------------------------------------------------------------------
class Light : public Object {
public:
	Light();
	Light(COLOR ambIn, COLOR matIn, int lighIDIn);
	virtual ~Light();
	void DrawPrimitives();

public:
	int lightID;
	vector3 direction;

public:
	void Load();
	void Initialize(COLOR ambIn, COLOR matIn, int lightIDIn);
	void SetDirection(vector3 &dir);
	void SetPosition(vector3 &pos);
};

#endif
