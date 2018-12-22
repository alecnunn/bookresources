/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "Camera.hpp"

#define MAX_SPANY			100		// Maximum spans we can have for one polygon
#define VERTEX_PER_SAMPLE	7.5f	// Conversion constant from grid space to GL

// Just in case it's already defined by the compiler
#ifdef min
 #undef min
#endif
#ifdef max
 #undef max
#endif

// Rounds a number to the grid's scale
#define min(a, b)	((a) < (b) ? (a) : (b))
#define max(a, b)	((a) > (b) ? (a) : (b))



// Structure to store X0 and Y0 given a Y
struct Span {
	short Start, End;
};



class Terrain {
public:
	Terrain();

	virtual bool		Update	(Camera &Cam);							// Updates the terrain
	virtual void		Draw	(Camera &Cam) = 0;						// Renders it

protected:
	Vector2D<float>		Center;											// Camera's Center rounded to grid space
	short				StartY, StopY;									// Starting & Ending Y
	Span				Strips[MAX_SPANY];								// Bounding polygon's set of strips
	float				TopY, BottomY, LeftX, RightX, FrontY, BackY;	// Clipping box values


private:
	bool Clip2D				(Vector2D<float> *Pts, unsigned short &Max);// Clips a 2D bounding frustrum
	bool Clip3D				(Vector3D<float> *Pts, unsigned short &Max);// Clips a set of 3D frustrum corners to 2 Y planes
	void GetBoundingPolygon	(Vector3D<float> *Src, Vector2D<float> *Pt, unsigned short &Max);	// Retrieves the bounding polygon
	void ScanLine			(Vector2D<float> *Pts, unsigned short Max);	// Scanlines the bounding polygon into <Strips[]>

	unsigned short			ID;											// Terrain's unique identifier
	static unsigned short	CurID;										// Class' current ID counter
};

#endif
