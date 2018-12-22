/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#include "Waves.hpp"
#include "GLImage.hpp"

#define HEIGHT_FUNCTION(X, Y, TIME) (10 + 10.0f * (float)sin((X) / 10.0f + (TIME) / 200.0f) * (float)cos((Y) / 10.0f - (TIME) / 300.0f) +		\
										  10.0f * (float)sin((X) / 30.0f + (TIME) / 100.0f) * (float)cos((Y) / 40.0f - (TIME) / 200.0f))

// Initializes the terrain's bounding box
Waves::Waves()
{
	LeftX	= -1000;
	RightX	= 1000;
	FrontY	= -1000;
	BackY	= 1000;
	BottomY = -10;
	TopY	= 40;	// 30 is the *true* value, but we want to cover 
}					// the entire span where the it snows



// Computers the height & Color of the terrain values at (X, Z)
bool Waves::Update(Camera &Cam, float Time)
{
	if (!DynamicTerrain::Update(Cam, Time))
		return false;


	Vector4D<float> *Col = Color;		// Current position in our color array
	Vector3D<float> *At  = Height;		// Current position in our vertex array

	// For every vertex registered in our vertex buffer,
	// calculate the height and the color at that location
	for (unsigned short i = MaxVert ; i ; i--) {
		At->Y = HEIGHT_FUNCTION(At->X - Center.X, At->Z - Center.Y, Time);

		Col->X = 0.6f - (At->Y - 10) / 160.0f;
		Col->Y = Col->X * 44.0f / 65.0f;
		Col->Z = Col->X * 30.0f / 64.0f;
		Col->W = 0.02f;

		At++;
		Col++;
	}

	return true;
}



// Renders the terrain
void Waves::Draw(Camera &Cam)
{
	// Add Color and alpha blending to our 
	// typical static terrain rendering function
	glEnableClientState	(GL_COLOR_ARRAY);
	glColorPointer		(4, GL_FLOAT, 0, &Color[0]);
	glColor3f			(1, 1, 1);
	glEnable			(GL_BLEND);
	glDisable			(GL_CULL_FACE);
	glDisable			(GL_DEPTH_TEST);
	glDisable			(GL_FOG);

	DynamicTerrain::Draw(Cam);

	// Reset to the old state
	glEnable		(GL_FOG);
	glEnable			(GL_DEPTH_TEST);
	glEnable			(GL_CULL_FACE);
	glDisable			(GL_BLEND);
	glEnable			(GL_CULL_FACE);
	glDisable			(GL_TEXTURE_GEN_S);
	glDisable			(GL_TEXTURE_GEN_T);	
	glDisableClientState(GL_COLOR_ARRAY);
}
