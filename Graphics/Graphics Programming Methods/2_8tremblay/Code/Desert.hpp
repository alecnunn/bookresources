/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#ifndef DESERT_HPP
#define DESERT_HPP

#include "StaticTerrain.hpp"

struct Desert : public StaticTerrain {
	 Desert();
	~Desert();

	virtual void Draw(Camera &Cam);					// Renders the terrain

protected:
	GLuint ID;										// Texture ID
	Vector2D<float>	Texture[MAX_HEIGHT][MAX_WIDTH];	// Texture values
};

#endif
