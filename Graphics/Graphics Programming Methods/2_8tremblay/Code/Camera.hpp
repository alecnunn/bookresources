/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Plane.hpp"

class Camera {
public:
	Vector3D<float>	Position;							// Camera's location in space
	Vector3D<float>	Rotation;							// Camera's rotation vector

	void Update				(void);						// Updates the frustrum planes
	void GetDirection		(Vector3D<float> &Dir);		// Retrieves the camera direction
	void GetFrustrumCorners	(Vector3D<float> *Pt);		// Retrieves the frustrum's corner
	void Transform			(Vector3D<float> *V);		// Transforms a vector from camera space

private:
	Plane3D		Front, Back, Left, Right, Bottom, Top;	// 6 planes of the viewing frustrum
};

#endif
