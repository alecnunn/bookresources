/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#ifndef DYNAMIC_TERRAIN_HPP
#define DYNAMIC_TERRAIN_HPP

#include "Terrain.hpp"

#define MAX_SPAN	5120					// Maximum possible # of vertex
#define MAX_INDEX	2 * (MAX_SPAN)			// Maximum possible # of index


struct DynamicTerrain : public Terrain {
	virtual bool		Update	(Camera &Cam, float Time);	// Updates the terrain
	virtual void		Draw	(Camera &Cam);				// Renders it

protected:
	Vector3D<float>	Height[MAX_SPAN];		// Vertex Height array
	unsigned short	Indices[MAX_INDEX];		// Matching vertex Index array
	unsigned short	MaxVert, MaxIndex;		// Total number of elements
};

#endif
