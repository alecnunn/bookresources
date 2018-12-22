/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef _STARS_H
#define _STARS_H
#include "object.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Stars : public Object {
public:
	Stars(int c, double r): cnt(c), radius(r), glListID(0) {};
	void DrawPrimitives() {
		static int first_time = 1;
		if (first_time) {
			Init();
			first_time = 0;
		}
		glCallList(glListID);
	}

private:
	void Init();
	int cnt;
	double radius;
	int glListID;
};

#endif
