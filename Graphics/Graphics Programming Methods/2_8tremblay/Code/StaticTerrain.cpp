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
#include "StaticTerrain.hpp"


// Set our bounding rectangle and pre-calculate
// our index table
StaticTerrain::StaticTerrain()
{
	unsigned short *Cur  = &Indices[0][0][0];	// Current position in our index array
	unsigned short Index = 0;					// Incremental index


	// Set our bounding region
	LeftX	= 0;
	RightX	= MAX_WIDTH - 2;
	FrontY	= 0;
	BackY	= MAX_HEIGHT - 2;

	// Calculate the indices between this span
	ASSERT(MAX_HEIGHT * MAX_WIDTH < 65536);
	for (unsigned short j = MAX_HEIGHT * MAX_WIDTH ; j ; j--, Index++) {
		*Cur++ = Index;
		*Cur++ = Index + MAX_HEIGHT;
	}
}



// Renders the terrain
void StaticTerrain::Draw(Camera &Cam)
{
	// The grid-rounded center of our terrain
	Vector2D<short> Center	= Vector2D<short>((short)(this->Center.X / VERTEX_PER_SAMPLE), (short)(this->Center.Y / VERTEX_PER_SAMPLE));
	Span *Strip				= Strips;			// Current position in our strip list

	glPushMatrix		();
	// Translate of the remainder of our grid translation
	glTranslatef		(Cam.Position.X, Cam.Position.Y, Cam.Position.Z);
	glEnableClientState	(GL_VERTEX_ARRAY);

	glVertexPointer		(3, GL_FLOAT, 0, &Height[0][0]);

	// For every Y, render the span stored in the Strip array
	for (short Y = StartY - Center.Y; Y <= StopY - Center.Y; Y++, Strip++)
		glDrawElements(GL_QUAD_STRIP, 2 * (Strip->End - Strip->Start + 1), 
					   GL_UNSIGNED_SHORT, &Indices[Y][Strip->Start- Center.X][0]);

	// Reset our state
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix			();
}

