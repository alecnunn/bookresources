/* Copyright (C) Eric Lengyel, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Lengyel, 2000"
 */
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

void LoadOffsetMatrix(GLdouble l, GLdouble r, GLdouble b, GLdouble t, 
					  GLdouble n, GLdouble f, GLfloat delta, GLfloat pz)
{
	GLfloat	matrix[16];
	
	// Set up standard perspective projection
	glMatrixMode(GL_PROJECTION);
	glFrustum(l, r, b, t, n, f);
	
	// Retrieve the projection matrix
	glGetFloatv(GL_PROJECTION_MATRIX, matrix);
	
	// Calculate epsilon with equation (7)
	GLfloat epsilon = -2.0F * f * n * delta / ((f + n) * pz * (pz + delta));
	
	// Modify entry (3,3) of the projection matrix
	matrix[10] *= 1.0F + epsilon;
	
	// Send the projection matrix back to OpenGL
	glLoadMatrixf(matrix);
}
