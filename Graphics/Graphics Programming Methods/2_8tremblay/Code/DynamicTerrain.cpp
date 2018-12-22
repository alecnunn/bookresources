/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#include <stdlib.h>

#include "DynamicTerrain.hpp"


// Computes the X & Z values for every X, Z values 
// inside the bounding polygon span
bool DynamicTerrain::Update(Camera &Cam, float Time)
{
	// First, isolate the bounding polygon
	if (!Terrain::Update(Cam))
		return false;


	// Now we can proceed with our grid evaluation
	Vector3D<float>	*At;				// Current position in our vertex array
	unsigned short	*IndAt = Indices;	// Current position in our index array
	Span			*Strip = Strips;	// Current position in our strip array
	unsigned short	 New, Old;			// Current and Old index value
	float			 X, Z;				// (X, Z) World space position
	short			 i;

	At   = Height;
	New  = 0;

	// Calculate (X, Z) for the first line
	Z = StartY		 * VERTEX_PER_SAMPLE;
	X = Strip->Start * VERTEX_PER_SAMPLE;
	for (i = Strip->End - Strip->Start + 1 ; i ; i--) {
		At->X = X;
		At->Z = Z;

		X += VERTEX_PER_SAMPLE;
		At++;
	}

	// Calculate (X, Z) for the remaining lines
	do {
		// First, calculate all the vertices for the next line
		Old = New;
		New	= At - Height;
		Z  += VERTEX_PER_SAMPLE;

		// We want the largest strip between the current and the next line
		X	= min(Strip->Start, (Strip + 1)->Start) * VERTEX_PER_SAMPLE;
		i	= max(Strip->End, (Strip + 1)->End) - min(Strip->Start, (Strip + 1)->Start);
		ASSERT(i >= 0);

		do {
			At->X = X;
			At->Z = Z;

			X += VERTEX_PER_SAMPLE;
			At++;
			ASSERT(At - Height < MAX_SPAN);
		} while (i--);

		// Then, add the indices for the row into the table
		unsigned short Top		= Old + (Strip->Start - (short)(Height[Old].X / VERTEX_PER_SAMPLE));
		unsigned short Bottom	= New + (Strip->Start - (short)(Height[New].X / VERTEX_PER_SAMPLE));

		for (i = Strip->End - Strip->Start + 1 ; i ; i--) {
			*IndAt++ = Top++;
			*IndAt++ = Bottom++;
			ASSERT(IndAt - Indices < MAX_INDEX);
		}

		Strip++;
	} while (Z <= StopY * VERTEX_PER_SAMPLE);

	// Calculate our maximums
	MaxVert  = At - Height;
	MaxIndex = IndAt - Indices;

	return true;
}



// Renders the terrain
void DynamicTerrain::Draw(Camera &Cam)
{
	Span *Strip			= Strips;	// Current position in our strip array
	unsigned short *At	= Indices;	// Current position in our index array

	glPushMatrix();
	// Translate of the remainder of our grid translation
	glTranslatef		(Cam.Position.X - Center.X, Cam.Position.Y, Cam.Position.Z - Center.Y);
	glEnableClientState	(GL_VERTEX_ARRAY);

	// Load our vertices
	glVertexPointer		(3, GL_FLOAT, 0, &Height[0].X);

	if (glLockArraysEXT)
		glLockArraysEXT	(0, MaxVert);

	// Render the X strip for every Y in our scanline buffer
	for (short Y = StartY ; Y <= StopY ; Y++, Strip++) {
		unsigned short Length = 2 * (Strip->End - Strip->Start + 1);

		glDrawElements(GL_QUAD_STRIP, Length, GL_UNSIGNED_SHORT, At);
		At += Length;
	}

	// Reset our state
	if (glUnlockArraysEXT)
		glUnlockArraysEXT();
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix			();
}
