/* Copyright (C) David Paull, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) David Paull, 2000"
 */
#ifndef _SPINNER_H
#define _SPINNER_H

#include "mtxlib.h"

const float SPINNER_ROT_DELTA = 0.5F;

class Spinner {
	float		mRot;
	matrix44	mRotMatrix;
	unsigned int mTextureID;

public:
	Spinner() {};
	~Spinner() {};
	void Init();
	void Display();
	void Update();

};

#endif
