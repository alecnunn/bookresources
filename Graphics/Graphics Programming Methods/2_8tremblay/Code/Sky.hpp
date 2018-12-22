/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#ifndef SKY_HPP
#define SKY_HPP

#include "DynamicTerrain.hpp"

struct Sky : public DynamicTerrain {
	 Sky();

	virtual bool Update	(Camera &Cam, float Time);			// Updates the terrain
	virtual void Draw	(Camera &Cam);						// Renders it

protected:
	Vector3D<float>	Color[MAX_SPAN];						// Color values
};

#endif
