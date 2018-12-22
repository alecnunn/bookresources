#include "cFrustum.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define DEGtoRAD(a) ( (a)*0.01745329252f )

cFrustum::cFrustum()
{

}

//--------------------------------------------------------------------

void cFrustum::SetPerspective( const float FOV, const float ViewAspect, const float nearZ, const float farZ )
{
    rFactor = tanf( DEGtoRAD(FOV/2) );
	uFactor = rFactor * ViewAspect;
	NearZ   = nearZ;
	FarZ    = farZ;
}

//--------------------------------------------------------------------

void cFrustum::Build(  const cVector3f& Eye, const cVector3f& Forward, const cVector3f& Right, const cVector3f& Up )
{
	EyePosition   = Eye;
    ForwardVector = Forward;
	RightVector   = Right;
	UpVector      = Up;
}

//--------------------------------------------------------------------

bool cFrustum::IsPointIn( const cVector3f& Point )
{
	cVector3f OP = Point - EyePosition;   

	float f = OP * ForwardVector;         // OP dot ForwardVector 
	if (f < NearZ || FarZ < f) return false;

	float r = OP * RightVector;              // OP dot RightVector
	float rLimit = rFactor * f;
	if (r < -rLimit || rLimit < r) return false;

	float u = OP * UpVector;              // OP dot RightVector
	float uLimit = uFactor * f;
	if (u < -uLimit || uLimit < u) return false;
	
    // Up to here the point is known to be in the frustum
    return true;
}

//--------------------------------------------------------------------
// This method is highly unoptimized and is presented here just as
// an example because it is not covered in the article but the algorithm
// is so similiar to the followed in the six-planes approach that it 
// need no much explanation.
// please, refer to http://fpuig.cjb.net/ for an optimized method that 
// use p-n-points and extend transformation.
bool cFrustum::IsAABBIn  ( const cVector3f& v1, const cVector3f& v2)
{
	cVector3f P;
	int nOutOfLeft=0, nOutOfRight=0, nOutOfFar=0, nOutOfNear=0, nOutOfTop=0, nOutOfBottom=0;
	bool bIsInRightTest, bIsInUpTest, bIsInFrontTest;

	cVector3f Corners[2];
	Corners[0] = v1-EyePosition;
	Corners[1] = v2-EyePosition;

	for (int i=0; i<8; ++i) {
		bIsInRightTest = bIsInUpTest = bIsInFrontTest = false;
		P.x=Corners[i&1].x;
		P.y=Corners[(i>>2)&1].y;
		P.z=Corners[(i>>1)&1].z;

		float r=RightVector.x*P.x + RightVector.y*P.y + RightVector.z*P.z;
		float u=UpVector.x*P.x + UpVector.y*P.y + UpVector.z*P.z;
		float f=ForwardVector.x*P.x + ForwardVector.y*P.y + ForwardVector.z*P.z;

		if (r< -rFactor*f) ++nOutOfLeft;
		else if (r > rFactor*f) ++nOutOfRight;
		else bIsInRightTest=true;
		
		if (u < -uFactor*f) ++nOutOfBottom;
		else if (u > uFactor*f) ++nOutOfTop;
		else bIsInUpTest=true;

		if (f < NearZ) ++nOutOfNear;		
		else if (f > FarZ) ++nOutOfFar;
		else bIsInFrontTest=true;

		if (bIsInRightTest && bIsInFrontTest && bIsInUpTest) return true;
	}

	if (nOutOfLeft==8 || nOutOfRight==8 || nOutOfFar==8 || nOutOfNear==8 || nOutOfTop==8 || nOutOfBottom==8 ) return false;
	return true;
}

//--------------------------------------------------------------------
// This method is highly unoptimized and is presented here just as
// an example because it is not covered in the article but the algorithm
// is so similiar to the followed in the six-planes approach that it 
// need no much explanation.
// please, refer to http://fpuig.cjb.net/ for an optimized method that 
// use p-n-points and extend transformation.

bool cFrustum::IsOBBIn   ( const cVector3f& Center, const cVector3f& HalfDimensions, const cQuaternionf& RotationQuat)
{

	cVector3f P;
	int nOutOfLeft=0, nOutOfRight=0, nOutOfFar=0, nOutOfNear=0, nOutOfTop=0, nOutOfBottom=0;
	bool bIsInRightTest, bIsInUpTest, bIsInFrontTest;

	cVector3f Corners[8];
	Corners[0].Set( Center.x+HalfDimensions.x,  Center.y+HalfDimensions.y,  Center.z+HalfDimensions.z);
	Corners[1].Set( Center.x+HalfDimensions.x,  Center.y+HalfDimensions.y,  Center.z-HalfDimensions.z);
	Corners[2].Set( Center.x+HalfDimensions.x,  Center.y-HalfDimensions.y,  Center.z+HalfDimensions.z);
	Corners[3].Set( Center.x+HalfDimensions.x,  Center.y-HalfDimensions.y,  Center.z-HalfDimensions.z);
	Corners[4].Set( Center.x-HalfDimensions.x,  Center.y+HalfDimensions.y,  Center.z+HalfDimensions.z);
	Corners[5].Set( Center.x-HalfDimensions.x,  Center.y+HalfDimensions.y,  Center.z-HalfDimensions.z);
	Corners[6].Set( Center.x-HalfDimensions.x,  Center.y-HalfDimensions.y,  Center.z+HalfDimensions.z);
	Corners[7].Set( Center.x-HalfDimensions.x,  Center.y-HalfDimensions.y,  Center.z-HalfDimensions.z);

	for (int i(0); i<8; ++i) Corners[i] -= EyePosition;

	for (i=0; i<8; ++i) {
		bIsInRightTest = bIsInUpTest = bIsInFrontTest = false;
		P=Corners[i];

		float r=RightVector.x*P.x + RightVector.y*P.y + RightVector.z*P.z;
		float u=UpVector.x*P.x + UpVector.y*P.y + UpVector.z*P.z;
		float f=ForwardVector.x*P.x + ForwardVector.y*P.y + ForwardVector.z*P.z;

		if (r< -rFactor*f) ++nOutOfLeft;
		else if (r > rFactor*f) ++nOutOfRight;
		else bIsInRightTest=true;
		
		if (u < -uFactor*f) ++nOutOfBottom;
		else if (u > uFactor*f) ++nOutOfTop;
		else bIsInUpTest=true;

		if (f < NearZ) ++nOutOfNear;		
		else if (f > FarZ) ++nOutOfFar;
		else bIsInFrontTest=true;

		if (bIsInRightTest && bIsInFrontTest && bIsInUpTest) return true;
	}

	if (nOutOfLeft==8 || nOutOfRight==8 || nOutOfFar==8 || nOutOfNear==8 || nOutOfTop==8 || nOutOfBottom==8 ) return false;
	return true;
}

//--------------------------------------------------------------------

bool cFrustum::IsHullIn( const cVector3f* aVertices, const char nVertices )
{
	int BehindLeft=0, BehindRight=0, BehindFar=0, BehindNear=0, BehindTop=0, BehindBottom=0;
	bool inForward, inRight, inUp;
	for (char i(0); i<nVertices; ++i)
	{
		inForward=inRight=inUp=false;

		cVector3f OP = aVertices[i] - EyePosition;   

		float f = OP * ForwardVector;
		float r = OP * RightVector;              // OP dot RightVector
		float u = OP * UpVector;              // OP dot RightVector

		float rLimit = rFactor * f;
		float uLimit = uFactor * f;

		if (f < NearZ ) ++BehindNear;
		else if (f > FarZ) ++BehindFar;
		else inForward=true;
		
		if (r < -rLimit) BehindLeft++;
		else if (r > rLimit) BehindRight++;
		else inRight=true;

		if (u < -uLimit) ++BehindBottom;		
		else if (u > uLimit) ++BehindTop;
		else inUp=true;

		if (inForward && inRight && inUp) return true;
	}
	
	if (BehindLeft==8 || BehindRight==8 || BehindFar==8 || BehindNear==8 || BehindTop==8 || BehindBottom==8) return false;
	return true;
}

//--------------------------------------------------------------------

bool  cFrustum::IsSphereIn( const cVector3f& Center, const float Radius )
{
	cVector3f OP = Center - EyePosition;   
	float f = OP * ForwardVector;         // OP dot ForwardVector 
	if (f < NearZ-Radius || FarZ+Radius < f) return false;

    // Unoptimized but more understandable
	float r = OP * RightVector; 
	float rLimit = rFactor * f;
	float rTop = rLimit + Radius;
	if (r < -rTop || rTop< r) return false;

    // Optimized ( a substraction is removed )
	float u = OP * UpVector; 
	float uLimit = uFactor * f;
	float uTop = uLimit + Radius;
	if (u < -uTop || uTop < u) return false;

	return true;
}
//--------------------------------------------------------------------

#include <windows.h>
#include <gl\gl.h>

void cFrustum::DrawOutline()
{
	cVector3f n1,n2,n3,n4;
	cVector3f f1,f2,f3,f4;
	cVector3f n = (ForwardVector * NearZ);
	cVector3f f = (ForwardVector * FarZ);
	n1 = n + RightVector * NearZ * rFactor  +  UpVector * NearZ * uFactor;
	n2 = n - RightVector * NearZ * rFactor  +  UpVector * NearZ * uFactor;
	n3 = n - RightVector * NearZ * rFactor  -  UpVector * NearZ * uFactor;
	n4 = n + RightVector * NearZ * rFactor  -  UpVector * NearZ * uFactor; 

	f1 = f + RightVector * FarZ * rFactor  +  UpVector * FarZ * uFactor;
	f2 = f - RightVector * FarZ * rFactor  +  UpVector * FarZ * uFactor;
	f3 = f - RightVector * FarZ * rFactor  -  UpVector * FarZ * uFactor;
	f4 = f + RightVector * FarZ * rFactor  -  UpVector * FarZ * uFactor;

	n1 += EyePosition;
	n2 += EyePosition;
	n3 += EyePosition;
	n4 += EyePosition;
	f1 += EyePosition;
	f2 += EyePosition;
	f3 += EyePosition;
	f4 += EyePosition;
	
	glBegin(GL_LINE_LOOP);
		glColor3f(1,1,1);
	    glVertex3fv( (float*)&n1 );
	    glVertex3fv( (float*)&n2 );
	    glVertex3fv( (float*)&n3 );
	    glVertex3fv( (float*)&n4 );
	    glVertex3fv( (float*)&n1 );
	glEnd();
	glBegin(GL_LINE_LOOP);
		glColor3f(1,1,1);
	    glVertex3fv( (float*)&f1 );
	    glVertex3fv( (float*)&f2 );
	    glVertex3fv( (float*)&f3 );
	    glVertex3fv( (float*)&f4 );
	    glVertex3fv( (float*)&f1 );
	glEnd();
	glBegin(GL_LINES);
		glColor3f(1,1,1);
	    glVertex3fv( (float*)&n1 );
	    glVertex3fv( (float*)&f1 );

	    glVertex3fv( (float*)&n2 );
	    glVertex3fv( (float*)&f2 );

		glVertex3fv( (float*)&n3 );
	    glVertex3fv( (float*)&f3 );

		glVertex3fv( (float*)&n4 );
	    glVertex3fv( (float*)&f4 );
	glEnd();
}
//--------------------------------------------------------------------

void cFrustum::DrawSolid()
{
	cVector3f n1,n2,n3,n4;
	cVector3f f1,f2,f3,f4;
	cVector3f n = (ForwardVector * NearZ);
	cVector3f f = (ForwardVector * FarZ);
	n1 = n + RightVector * NearZ * rFactor  +  UpVector * NearZ * uFactor;
	n2 = n - RightVector * NearZ * rFactor  +  UpVector * NearZ * uFactor;
	n3 = n - RightVector * NearZ * rFactor  -  UpVector * NearZ * uFactor;
	n4 = n + RightVector * NearZ * rFactor  -  UpVector * NearZ * uFactor; 

	f1 = f + RightVector * FarZ * rFactor  +  UpVector * FarZ * uFactor;
	f2 = f - RightVector * FarZ * rFactor  +  UpVector * FarZ * uFactor;
	f3 = f - RightVector * FarZ * rFactor  -  UpVector * FarZ * uFactor;
	f4 = f + RightVector * FarZ * rFactor  -  UpVector * FarZ * uFactor;

	n1 += EyePosition;
	n2 += EyePosition;
	n3 += EyePosition;
	n4 += EyePosition;
	f1 += EyePosition;
	f2 += EyePosition;
	f3 += EyePosition;
	f4 += EyePosition;

	//Draw first a white outline
	glColor4f(1,1,1,1);
	glBegin(GL_LINE_LOOP);
		glColor3f(1,1,1);
	    glVertex3fv( (float*)&n1 );
	    glVertex3fv( (float*)&n2 );
	    glVertex3fv( (float*)&n3 );
	    glVertex3fv( (float*)&n4 );
	    glVertex3fv( (float*)&n1 );
	glEnd();
	glBegin(GL_LINE_LOOP);
		glColor3f(1,1,1);
	    glVertex3fv( (float*)&f1 );
	    glVertex3fv( (float*)&f2 );
	    glVertex3fv( (float*)&f3 );
	    glVertex3fv( (float*)&f4 );
	    glVertex3fv( (float*)&f1 );
	glEnd();
	glBegin(GL_LINES);
		glColor3f(1,1,1);
	    glVertex3fv( (float*)&n1 );
	    glVertex3fv( (float*)&f1 );

	    glVertex3fv( (float*)&n2 );
	    glVertex3fv( (float*)&f2 );

		glVertex3fv( (float*)&n3 );
	    glVertex3fv( (float*)&f3 );

		glVertex3fv( (float*)&n4 );
	    glVertex3fv( (float*)&f4 );
	glEnd();


	// And next draw the transparent polygons

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,0.2f);
	glBegin(GL_QUADS);
	    glVertex3fv( (float*)&n1 );
	    glVertex3fv( (float*)&n2 );
	    glVertex3fv( (float*)&n3 );
	    glVertex3fv( (float*)&n4 );
	glEnd();
	glBegin(GL_QUADS);
	    glVertex3fv( (float*)&f1 );
	    glVertex3fv( (float*)&f2 );
	    glVertex3fv( (float*)&f3 );
	    glVertex3fv( (float*)&f4 );
	glEnd();
	glBegin(GL_QUADS);
	    glVertex3fv( (float*)&f1 );
	    glVertex3fv( (float*)&f2 );
	    glVertex3fv( (float*)&n2 );
	    glVertex3fv( (float*)&n1 );
	glEnd();
	glBegin(GL_QUADS);
	    glVertex3fv( (float*)&f3 );
	    glVertex3fv( (float*)&f2 );
	    glVertex3fv( (float*)&n2 );
	    glVertex3fv( (float*)&n3 );
	glEnd();
	glBegin(GL_QUADS);
	    glVertex3fv( (float*)&f3 );
	    glVertex3fv( (float*)&f4 );
	    glVertex3fv( (float*)&n4 );
	    glVertex3fv( (float*)&n3 );
	glEnd();
	glBegin(GL_QUADS);
	    glVertex3fv( (float*)&f1 );
	    glVertex3fv( (float*)&f4 );
	    glVertex3fv( (float*)&n4 );
	    glVertex3fv( (float*)&n1 );
	glEnd();
	glDisable(GL_BLEND);
}
