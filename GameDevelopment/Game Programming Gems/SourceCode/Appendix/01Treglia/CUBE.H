/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef _CUBE_H
#define _CUBE_H
#include "def.h"
#include <cstdlib>

//---------------------------------------------------------------------------
// The Cube
//---------------------------------------------------------------------------
class Cube {
public:
	Cube(u16 in_w, u16 in_h, u16 in_l);
	void DrawPrimitives();

public:
	// Width x Hieght x Length
	u16 w;
	u16 h;
	u16 l;
};
#endif
