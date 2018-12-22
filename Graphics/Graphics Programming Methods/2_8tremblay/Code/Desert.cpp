/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#include "GLImage.hpp"
#include "Desert.hpp"


// Initializes the terrain's data
Desert::Desert()
{
	// For every values in the grid;
	for (unsigned short j = 0 ; j < MAX_HEIGHT ; j++)
		for (unsigned short i = 0 ; i < MAX_WIDTH ; i++) {
			// Set the height at (j, i)
			Height[j][i]  = Vector3D<float>(VERTEX_PER_SAMPLE * i, -10.0f + 10.0f * 
				(float)sin(j / 10.0) * (float)cos(i / 10.0), VERTEX_PER_SAMPLE * j);

			// and calculate set the texture value at (j, i)
			Texture[j][i] = Vector2D<float>((float)sin(i / 10.0) * (float)sin(j / 10.0) + 
											(float)cos(j / 10.0) * (float)cos(i / 10.0) / 2.0f + 0.5f, 
											(float)cos(j / 10.0) * (float)cos(j / 10.0) + 
											(float)sin(j / 10.0) * (float)sin(i / 10.0) / 2.0f + 0.5f);
		}


	// Set the 2 Y Clipping planes
	BottomY = -20;
	TopY	= 0;


	// Load our texture
	ID = LoadGLImage("PlanetGround.bmp");

	ASSERT((ID >= 0) && (ID != -1));
}



// Deinitialize the terrain
Desert::~Desert()
{
	glDeleteTextures(1, &ID);
}



// Renders the terrain
void Desert::Draw(Camera &Cam)
{
	// Add texturing to our typical static terrain rendering function
	glEnable			(GL_TEXTURE_2D);
	glEnableClientState	(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer	(2, GL_FLOAT, 0, &Texture[0][0]);
	glBindTexture		(GL_TEXTURE_2D, ID);
	glColor3f			(1, 1, 1);

	StaticTerrain::Draw	(Cam);

	// Reset to the old state
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable			(GL_TEXTURE_2D);
}

