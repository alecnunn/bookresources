/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
// Conventional static lighting code.


#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "mtxlib.h"
#include "mcolor.h"



typedef struct {
	vector3 Pos;			// This vertex's 3D position in model-space
	Color_t StaticColor;	// RGB to be used as Gouraud for this vertex
} Vertex_t;


void DrawMyTriangle( const Vertex_t *pV1, const Vertex_t *pV2, const Vertex_t *pV3 ) {
	glBegin( GL_TRIANGLES );

	glColor3f( pV1->StaticColor.fR, pV1->StaticColor.fG, pV1->StaticColor.fB );
	glVertex3f( pV1->Pos.x, pV1->Pos.y, pV1->Pos.z );

	glColor3f( pV2->StaticColor.fR, pV2->StaticColor.fG, pV2->StaticColor.fB );
	glVertex3f( pV2->Pos.x, pV2->Pos.y, pV2->Pos.z );

	glColor3f( pV3->StaticColor.fR, pV3->StaticColor.fG, pV3->StaticColor.fB );
	glVertex3f( pV3->Pos.x, pV3->Pos.y, pV3->Pos.z );

	glEnd();
}

