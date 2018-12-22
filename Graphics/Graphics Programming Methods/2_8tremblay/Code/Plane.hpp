/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved

	This is our 3D Plane object.  It stores the plane in a
	format given by Xx + Yy + Zz + W = 0.
*/
#ifndef PLANE3D_HPP
#define PLANE3D_HPP

#include "Vector.hpp"

// Because doubles are rational numbers, we must make 
// sure that the error is reasonable
#define THRESHOLD 0.000001

// Some compilers define this as ABS(int) so we overide it :)
#define ABS(x) ((x) < 0 ? -(x) : (x))

struct Plane3D : Vector4D<double> {
	Plane3D			() {};

	Plane3D			(double A, double B, double C, double D) : Vector4D<double>(A, B, C, D) {}


	// Sets the value of the plane
	void Set		(double A, double B, double C, double D)
	{
		X = A;
		Y = B;
		Z = C;
		W = D;
	}



	// Calculates the intersection of 3 planes and stores the vertex
	void Intersect	(Plane3D *S, Plane3D *T, Vector3D<float> &Pt)
	{
		// We need doubles here or else the asserts will pop-up
		Vector3D<double> N = *(Vector3D<double> *)S / *(Vector3D<double> *)T;
		Vector3D<double> Temp;

		// Calculate the intersection point
		Temp = (N * W +
			  (*(Vector3D<double> *)T		/ *(Vector3D<double> *)this) * S->W +
			  (*(Vector3D<double> *)this	/ *(Vector3D<double> *)S)    * T->W)/
			  -(*(Vector3D<double> *)this * N);

		// Make sure that what we got is roughly what it should be
		ASSERT(ABS(   X * Temp.X +    Y * Temp.Y +    Z * Temp.Z +    W) <= THRESHOLD);
		ASSERT(ABS(S->X * Temp.X + S->Y * Temp.Y + S->Z * Temp.Z + S->W) <= THRESHOLD);
		ASSERT(ABS(T->X * Temp.X + T->Y * Temp.Y + T->Z * Temp.Z + T->W) <= THRESHOLD);

		// Convert the intersection point in floats
		Pt.X = (float)Temp.X;
		Pt.Y = (float)Temp.Y;
		Pt.Z = (float)Temp.Z;
	}



	// Normalizes the plane
	void Normalize(void)
	{
		double n = Vector3D<double>::Norm();

		X /= n;
		Y /= n;
		Z /= n;
		W /= n;
	}
};

#endif
