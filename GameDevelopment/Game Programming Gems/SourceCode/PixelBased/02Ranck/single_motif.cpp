/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
// Code to generate a single motif per vertex.

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "mtxlib.h"
#include "mcolor.h"
#include "motif_table.h"



typedef struct {
	vector3 Pos;		// This vertex's 3D position in model-space
	int nMotifIndex;	// Index into the motif table
	float fIntensity;	// Intensity of the light motif's RGB at this vertex
} Vertex_t;


void DrawMyTriangle( const Vertex_t *pV1, const Vertex_t *pV2, const Vertex_t *pV3 ) {
	Color_t *pColor;
	float fR, fG, fB;

	glBegin( GL_TRIANGLES );

	pColor = &aMotifTable[ pV1->nMotifIndex ];
	fR = pColor->fR * pV1->fIntensity;
	fG = pColor->fG * pV1->fIntensity;
	fB = pColor->fB * pV1->fIntensity;
	glColor3f( fR, fG, fB );
	glVertex3f( pV1->Pos.x, pV1->Pos.y, pV1->Pos.z );

	pColor = &aMotifTable[ pV2->nMotifIndex ];
	fR = pColor->fR * pV2->fIntensity;
	fG = pColor->fG * pV2->fIntensity;
	fB = pColor->fB * pV2->fIntensity;
	glColor3f( fR, fG, fB );
	glVertex3f( pV2->Pos.x, pV2->Pos.y, pV2->Pos.z );

	pColor = &aMotifTable[ pV3->nMotifIndex ];
	fR = pColor->fR * pV3->fIntensity;
	fG = pColor->fG * pV3->fIntensity;
	fB = pColor->fB * pV3->fIntensity;
	glColor3f( fR, fG, fB );
	glVertex3f( pV3->Pos.x, pV3->Pos.y, pV3->Pos.z );

	glEnd();
}

