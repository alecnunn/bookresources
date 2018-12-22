/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "cube.h"

//---------------------------------------------------------------------------
// Constant Data
//---------------------------------------------------------------------------
// Face Texture Coords
static float cubeTexCoord[8] = {
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
};
// Face Info
static float cubeVtxSign[8][3] = {
	{-1,  1,  1},
	{ 1,  1,  1},
	{ 1, -1,  1},
	{-1, -1,  1},
	{-1,  1, -1},
	{ 1,  1, -1},
	{ 1, -1, -1},
	{-1, -1, -1},
};
// Face Info
static int cubeFace[6][4] = {
	// Front
	{0, 1, 2, 3},
	// Right
	{1, 5, 6, 2},
	// Back
	{5, 4, 7, 6},
	// Left
	{4, 0, 3, 7},
	// Top
	{4, 5, 1, 0},
	// Bottom
	{3, 2, 6, 7},
};
// Face Normals
static f32 cubeFaceNrm[6][3] = {
	// Front
	{0.0, 0.0, 1.0},
	// Right
	{1.0, 0.0, 0.0},
	// Back
	{0.0, 0.0, -1.0},
	// Left
	{-1.0, 0.0, 0.0},
	// Top
	{0.0, 1.0, 0.0},
	// Bottom
	{0.0, -1.0, 0.0},
};
// Initial Face Colors
static float cubeFaceClr[24] = {
	// Front
	1.0, 0.0, 0.0, 1.0,
	// Right
	0.0, 1.0, 0.0, 1.0,
	// Back
	1.0, 0.0, 0.0, 1.0,
	// Left
	0.0, 1.0, 0.0, 1.0,
	// Top
	0.0, 0.0, 1.0, 1.0,
	// Bottom
	0.0, 0.0, 1.0, 1.0,
};

//---------------------------------------------------------------------------
// Cube 
//---------------------------------------------------------------------------
Cube::Cube(u16 cW, u16 cH, u16 cL) {
	// Initialize Structure
	w = cW;
	h = cH;
	l = cL;
}

//---------------------------------------------------------------------------
// Draw
//---------------------------------------------------------------------------
void Cube::DrawPrimitives() {
	u32 i, j;

	// Draw a cube
	for (i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glColor4f(
			cubeFaceClr[i * 4 + 0],
			cubeFaceClr[i * 4 + 1],
			cubeFaceClr[i * 4 + 2],
			cubeFaceClr[i * 4 + 3]
		);
		for (j = 0; j < 4; j++) {
			glTexCoord2f(cubeTexCoord[2 * j + 0], cubeTexCoord[2 * j + 1]);
			glVertex3f(
				w * (float)cubeVtxSign[cubeFace[i][j]][0],
				h * (float)cubeVtxSign[cubeFace[i][j]][1],
				l * (float)cubeVtxSign[cubeFace[i][j]][2]
			);
		}
		glEnd();
	}
}

