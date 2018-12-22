/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#include "plane.h"

//---------------------------------------------------------------------------
// Plane Constructor
//---------------------------------------------------------------------------
Plane::Plane(float tileXSizeIn, float tileYSizeIn, int tileXNumIn, 
		int tileYNumIn, int texXNumIn, int texYNumIn, COLOR colorIn) {

	// Init Parameters
	tileXSize = tileXSizeIn;
	tileXNum = tileXNumIn;
	xSize = tileXNum * tileXSize;
	tileYSize = tileYSizeIn;
	tileYNum = tileYNumIn;
	ySize = tileYNum * tileYSize;
	color = colorIn;

	vtxCnt = (tileXNum + 1) * (tileYNum + 1);

	normal.x = 0.0;
	normal.y = 1.0;
	normal.z = 0.0;
}


//---------------------------------------------------------------------------
// Draw
//---------------------------------------------------------------------------
void Plane::DrawPrimitives() {
    int x, y, i;

    glColor4ub(color.r, color.g, color.b, color.a);
	if (lightsOn) {
		glNormal3f(
			normal.x,
			normal.y,
			normal.z
		);
	}
    for (y = 0; y < tileYNum; ++y) {
        i = y * tileXNum;
        glBegin(GL_TRIANGLE_STRIP);
        //glBegin(GL_POINTS);
            for (x = 0 ;x < tileXNum + 1; ++x) {
				glVertex3f(
					(float)(x * tileXSize - (xSize / 2.0)), 
					0.0, 
					(float)(y * tileYSize - (ySize / 2.0))
				);
				glVertex3f(
					(float)(x * tileXSize - (xSize / 2.0)), 
					0.0, 
					(float)((y+1) * tileYSize - (ySize / 2.0))
				);
            }
        glEnd();
    }
}

