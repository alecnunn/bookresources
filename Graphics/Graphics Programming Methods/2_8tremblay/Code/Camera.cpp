/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved

	If you're looking at speeding this up, 
	this is where SIMD instructions would be very usefull
*/
#include "Camera.hpp"


// Updates the camera's frustrum planes
void Camera::Update(void)
{
	double Proj[16], ModView[16], Result[16];

	// Retrieve the matrices
	glGetDoublev(GL_PROJECTION_MATRIX, Proj);
	glGetDoublev(GL_MODELVIEW_MATRIX,  ModView);

	// Multily the matrixes in the same order OpenGL does it
	Result[ 0] = ModView[ 0] * Proj[ 0] + ModView[ 1] * Proj[ 4] + 
				 ModView[ 2] * Proj[ 8] + ModView[ 3] * Proj[12];
	Result[ 1] = ModView[ 0] * Proj[ 1] + ModView[ 1] * Proj[ 5] + 
				 ModView[ 2] * Proj[ 9] + ModView[ 3] * Proj[13];
	Result[ 2] = ModView[ 0] * Proj[ 2] + ModView[ 1] * Proj[ 6] + 
				 ModView[ 2] * Proj[10] + ModView[ 3] * Proj[14];
	Result[ 3] = ModView[ 0] * Proj[ 3] + ModView[ 1] * Proj[ 7] + 
				 ModView[ 2] * Proj[11] + ModView[ 3] * Proj[15];

	Result[ 4] = ModView[ 4] * Proj[ 0] + ModView[ 5] * Proj[ 4] + 
				 ModView[ 6] * Proj[ 8] + ModView[ 7] * Proj[12];
	Result[ 5] = ModView[ 4] * Proj[ 1] + ModView[ 5] * Proj[ 5] + 
				 ModView[ 6] * Proj[ 9] + ModView[ 7] * Proj[13];
	Result[ 6] = ModView[ 4] * Proj[ 2] + ModView[ 5] * Proj[ 6] + 
				 ModView[ 6] * Proj[10] + ModView[ 7] * Proj[14];
	Result[ 7] = ModView[ 4] * Proj[ 3] + ModView[ 5] * Proj[ 7] + 
				 ModView[ 6] * Proj[11] + ModView[ 7] * Proj[15];

	Result[ 8] = ModView[ 8] * Proj[ 0] + ModView[ 9] * Proj[ 4] + 
				 ModView[10] * Proj[ 8] + ModView[11] * Proj[12];
	Result[ 9] = ModView[ 8] * Proj[ 1] + ModView[ 9] * Proj[ 5] + 
				 ModView[10] * Proj[ 9] + ModView[11] * Proj[13];
	Result[10] = ModView[ 8] * Proj[ 2] + ModView[ 9] * Proj[ 6] + 
				 ModView[10] * Proj[10] + ModView[11] * Proj[14];
	Result[11] = ModView[ 8] * Proj[ 3] + ModView[ 9] * Proj[ 7] + 
				 ModView[10] * Proj[11] + ModView[11] * Proj[15];

	Result[12] = ModView[12] * Proj[ 0] + ModView[13] * Proj[ 4] + 
				 ModView[14] * Proj[ 8] + ModView[15] * Proj[12];
	Result[13] = ModView[12] * Proj[ 1] + ModView[13] * Proj[ 5] + 
				 ModView[14] * Proj[ 9] + ModView[15] * Proj[13];
	Result[14] = ModView[12] * Proj[ 2] + ModView[13] * Proj[ 6] + 
				 ModView[14] * Proj[10] + ModView[15] * Proj[14];
	Result[15] = ModView[12] * Proj[ 3] + ModView[13] * Proj[ 7] + 
				 ModView[14] * Proj[11] + ModView[15] * Proj[15];

	// Calculate the plane equations given the tranformation
	Right.X		= Result[ 3] - Result[ 0];
	Right.Y		= Result[ 7] - Result[ 4];
	Right.Z		= Result[11] - Result[ 8];
	Right.W		= Result[15] - Result[12];
	Right.Normalize();

	Left.X		= Result[ 3] + Result[ 0];
	Left.Y		= Result[ 7] + Result[ 4];
	Left.Z		= Result[11] + Result[ 8];
	Left.W		= Result[15] + Result[12];
	Left.Normalize();

	Top.X		= Result[ 3] - Result[ 1];
	Top.Y		= Result[ 7] - Result[ 5];
	Top.Z		= Result[11] - Result[ 9];
	Top.W		= Result[15] - Result[13];
	Top.Normalize();

	Bottom.X	= Result[ 3] + Result[ 1];
	Bottom.Y	= Result[ 7] + Result[ 5];
	Bottom.Z	= Result[11] + Result[ 9];
	Bottom.W	= Result[15] + Result[13];
	Bottom.Normalize();

	Back.X		= Result[ 3] - Result[ 2];
	Back.Y		= Result[ 7] - Result[ 6];
	Back.Z		= Result[11] - Result[10];
	Back.W		= Result[15] - Result[14];
	Back.Normalize();

	Front.X		= Result[ 3] + Result[ 2];
	Front.Y		= Result[ 7] + Result[ 6];
	Front.Z		= Result[11] + Result[10];
	Front.W		= Result[15] + Result[14];
	Front.Normalize();
}



// Transforms a vector from camera space to world space
void Camera::Transform(Vector3D<float> *V)
{
	float ModView[16];
	float VX, VY, VZ;

	glGetFloatv(GL_MODELVIEW_MATRIX, ModView);

	// Muiltiply our vector by the transformation
	VX = V->X * ModView[ 0] + V->Y * ModView[ 1] + V->Z * ModView[ 2];
	VY = V->X * ModView[ 4] + V->Y * ModView[ 5] + V->Z * ModView[ 6];
	VZ = V->X * ModView[ 8] + V->Y * ModView[ 9] + V->Z * ModView[10];

	V->X = VX;
	V->Y = VY;
	V->Z = VZ;
}



// Returns the direction the camera is looking at
void Camera::GetDirection(Vector3D<float> &Dir)
{
	float ModView[16];

	// Retrieve the transformation matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, ModView);

	Dir.X = ModView[2];
	Dir.Y = ModView[2 + 4];
	Dir.Z = ModView[2 + 8];
}



// Returns the 8 frustrum bounding corners
void Camera::GetFrustrumCorners(Vector3D<float> *Pt)
{
	// Calculate the intersection of the planes 3 by 3
	Front.Intersect(&Left,  &Top,	 *Pt++);
	Front.Intersect(&Left,  &Bottom, *Pt++);
	Front.Intersect(&Right, &Top,	 *Pt++);
	Front.Intersect(&Right, &Bottom, *Pt++);
	Back.Intersect(&Left,   &Top,	 *Pt++);
	Back.Intersect(&Left,   &Bottom, *Pt++);
	Back.Intersect(&Right,  &Top,	 *Pt++);
	Back.Intersect(&Right,  &Bottom, *Pt);


	// Position the Y for our 3D clipping to work!
	for (unsigned short i = 8 ; i ; i--)
		Pt--->Y -= this->Position.Y;
}

