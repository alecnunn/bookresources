/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#ifndef STATIC_TERRAIN_HPP
#define STATIC_TERRAIN_HPP

#include "Terrain.hpp"

#define MAX_WIDTH	100
#define MAX_HEIGHT	100


struct StaticTerrain : public Terrain {
	StaticTerrain();
	virtual void Draw(Camera &Cam);						// Renders the terrain

protected:
	Vector3D<float>	Height[MAX_HEIGHT][MAX_WIDTH];		// Vertex Height array
	unsigned short	Indices[MAX_HEIGHT][MAX_WIDTH][2];	// Matching vertex Index array
};

#endif
