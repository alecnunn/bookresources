/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
// Code to generate multiple motifs per vertex.

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "mtxlib.h"
#include "mcolor.h"
#include "motif_table.h"


#define MAX_MOTIFS_PER_VTX	8	// Implementation-specific value


typedef struct {
	int nMotifIndex;	// Index into the motif table
	float fIntensity;	// Intensity of the light motif's RGB at this vertex
} MotifEntry_t;


typedef struct {
	vector3 Pos;				// This vertex's 3D position in model-space
	int nMotifEntryCount;		// Number of motif entries in aMotifEntry[]
	MotifEntry_t aMotifEntry[MAX_MOTIFS_PER_VTX];
} Vertex_t;


void ComputeVertexColor( const Vertex_t *pV, Color_t *pColor ) {
	Color_t *pMotifColor;
	float fR, fG, fB;
	int i;

	// Zero color components: 
	fR = fG = fB = 0.0f;

	// Step through all the motifs affecting this vertex
	//   and sum their colors:
	for( i=0; i<pV->nMotifEntryCount; i++ ) {
		pMotifColor = &aMotifTable[ pV->aMotifEntry[i].nMotifIndex ];
		fR += pMotifColor->fR * pV->aMotifEntry[i].fIntensity;
		fG += pMotifColor->fG * pV->aMotifEntry[i].fIntensity;
		fB += pMotifColor->fB * pV->aMotifEntry[i].fIntensity;
	}

	// Make sure final color is from 0 to 1:
	if( fR > 1.0f ) fR = 1.0f;
	if( fG > 1.0f ) fG = 1.0f;
	if( fB > 1.0f ) fB = 1.0f;

	// Store final colors in return variable:
	pColor->fR = fR;
	pColor->fG = fG;
	pColor->fB = fB;
}

void DrawMyTriangle( const Vertex_t *pV1, const Vertex_t *pV2, const Vertex_t *pV3 ) {
	Color_t Color;

	glBegin( GL_TRIANGLES );

	ComputeVertexColor( pV1, &Color );
	glColor3f( Color.fR, Color.fG, Color.fB );
	glVertex3f( pV1->Pos.x, pV1->Pos.y, pV1->Pos.z );

	ComputeVertexColor( pV2, &Color );
	glColor3f( Color.fR, Color.fG, Color.fB );
	glVertex3f( pV2->Pos.x, pV2->Pos.y, pV2->Pos.z );

	ComputeVertexColor( pV3, &Color );
	glColor3f( Color.fR, Color.fG, Color.fB );
	glVertex3f( pV3->Pos.x, pV3->Pos.y, pV3->Pos.z );

	glEnd();
}
