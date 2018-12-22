// cFrustum.h: interface for the cFrustum class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CFRUSTUM_H
#define CFRUSTUM_H


#include "cQuaternionf.h"

class cFrustum  
{
	public:
		cFrustum();

		void SetPerspective( const float FOV, const float ViewAspect, const float NearZ, const float FarZ );
		void Build( const cVector3f& Eye, const cVector3f& Forward, const cVector3f& Right, const cVector3f& Up );

		bool IsPointIn ( const cVector3f& Point );
		bool IsHullIn  ( const cVector3f* aVertices, const char nVertices );
		bool IsSphereIn( const cVector3f& Center, const float Radius );
		bool IsAABBIn  ( const cVector3f& v1, const cVector3f& v2);
		bool IsOBBIn   ( const cVector3f& Center, const cVector3f& HalfDimensions, const cQuaternionf& RotationQuat);

		void DrawOutline();
		void DrawSolid();

	private:
		cVector3f EyePosition,
				  ForwardVector,
				  RightVector,
				  UpVector;
				
		float rFactor, 
			  uFactor;

		float NearZ, 
			  FarZ;
};

#endif
