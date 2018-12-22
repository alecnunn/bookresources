/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#include "Sky.hpp"
#include "GLImage.hpp"

#define HEIGHT_FUNCTION(X, Y, TIME) (60 + 10.0f * (float)sin((Y) / (10.0f * VERTEX_PER_SAMPLE)) * (float)cos((X) / (10.0f * VERTEX_PER_SAMPLE)) +		\
										  10.0f * (float)sin((X) / 30.0f + (TIME) / 1000.0f) * (float)cos((Y) / 40.0f - (TIME) / 2000.0f))

// Initializes the terrain's bounding box
Sky::Sky()
{
	LeftX	= -1000;
	RightX	= 1000;
	FrontY	= -1000;
	BackY	= 1000;
	BottomY = 40;
	TopY	= 80;
}



// Computers the Height & Color of the terrain values at (X, Z)
bool Sky::Update(Camera &Cam, float Time)
{
	if (!DynamicTerrain::Update(Cam, Time))
		return false;


	Vector3D<float> *At  = Height;	// Current Vertex in our vertex array
	Vector3D<float> *Col = Color;	// Current Color in our color array

	// For every vertex registered in our vertex buffer,
	// calculate the height and the color at that location
	for (unsigned short i = MaxVert ; i ; i--) {
		At->Y = HEIGHT_FUNCTION(At->X - Center.X, At->Z - Center.Y, Time);

		Col->Z = 0.6f - (At->Y - 60) / 80.0f;
		Col->Y = Col->Z * 44.0f / 65.0f;
		Col->X = Col->Z * 30.0f / 64.0f;

		At++;
		Col++;
	}

	return true;
}


// Renders the terrain
void Sky::Draw(Camera &Cam)
{
	// Add color and swap the winding rule
	glCullFace			(GL_FRONT);
	glEnableClientState	(GL_COLOR_ARRAY);
	glColorPointer		(3, GL_FLOAT, 0, &Color[0]);
	glColor3f			(1, 1, 1);

	DynamicTerrain::Draw(Cam);

	// Reset to the old state
	glDisableClientState(GL_COLOR_ARRAY);
	glCullFace			(GL_BACK);
	glDisable			(GL_BLEND);

}
