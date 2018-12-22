/* Copyright (C) Tim Round, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tim Round, 2000"
 */
#ifdef _WIN32
#include <windows.h>
#endif
#include <math.h>

#ifndef _WIN32
typedef unsigned int BOOL;
#define TRUE 1
#define FALSE 0
#endif

typedef struct _VECTOR
{
	float x;
	float y;
	float z;
}VECTOR;

typedef struct _PLANE
{
	float a;
	float b;
	float c;
	float d;
}PLANE;

typedef struct _MATRIX
{
	float _11;
	float _12;
	float _13;
	float _14;
	float _21;
	float _22;
	float _23;
	float _24;
	float _31;
	float _32;
	float _33;
	float _34;
	float _41;
	float _42;
	float _43;
	float _44;
}MATRIX;

PLANE	g_FOVLeftPlane;
PLANE	g_FOVRightPlane;
PLANE	g_FOVTopPlane;
PLANE	g_FOVBottomPlane;
float	g_FOVFrontClip;
float	g_FOVBackClip;
MATRIX	g_ViewTransform;


void	Normalize(VECTOR *pV)
{
	float Length , InvLength;

	Length = (float) sqrt( ( pV->x * pV->x ) + ( pV->y * pV->y ) + ( pV->z * pV->z ) );
	InvLength = 1.0f / Length;
	pV->x /= InvLength;
	pV->y /= InvLength;
	pV->z /= InvLength;
}


void	CrossProduct(VECTOR *pV0 , VECTOR *pV1 , VECTOR *pCrossProduct)
{
	pCrossProduct->x = pV0->y * pV1->z - pV0->z * pV1->y;
	pCrossProduct->y = pV0->z * pV1->x - pV0->x * pV1->z;
	pCrossProduct->z= pV0->x * pV1->y - pV0->y * pV1->x;
}


void	PlaneFromPoints(VECTOR *pP0 , VECTOR *pP1 , VECTOR *pP2 , PLANE *pPlane)
{
	VECTOR V0,V1,V2;

	V0.x = pP1->x - pP0->x;
	V0.y = pP1->y - pP0->y;
	V0.z= pP1->z- pP0->z;
	V1.x = pP2->x - pP0->x;
	V1.y = pP2->y - pP0->y;
	V1.z= pP2->z- pP0->z;
	CrossProduct( &V0, &V1, &V2);
	Normalize( &V2);
	pPlane->a = V2.x;
	pPlane->b = V2.y;
	pPlane->c = V2.z;
	pPlane->d = -( V2.x * pP0->x + V2.y * pP0->y + V2.z * pP0->z );
}


//This function calculates the planes for describing the view frustum using 3 points.
//Because we are in view space the cameras position is at 0,0,0
//We use the back clip position and the viewing angle to work out a point on the edge 
//of the frustum.
//The view angle is the angle between the top and bottom of the view frustum in radians.

void	SetupFOVClipPlanes( float Angle , float Aspect , float FrontClip , float BackClip )
{
	VECTOR	P0 , P1 , P2;

//	Calculate left plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = -BackClip * ( (float)tan( Angle * 0.5f ) / Aspect );
	P1.y = -BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z = BackClip;
	P2.x = P1.x;
	P2.y = -P1.y;
	P2.z = P1.z;
	PlaneFromPoints( &P0, &P1 , &P2 , &g_FOVLeftPlane );

//	Calculate right plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = BackClip * ( (float)tan( Angle * 0.5f ) / Aspect);
	P1.y = BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z = BackClip;
	P2.x = P1.x;
	P2.y = -P1.y;
	P2.z = P1.z;
	PlaneFromPoints( &P0, &P1 , &P2 , &g_FOVRightPlane );

//	Calculate top plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = -BackClip * ( (float)tan( Angle * 0.5f ) / Aspect);
	P1.y = BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z = BackClip;
	P2.x = -P1.x;
	P2.y = P1.y;
	P2.z = P1.z;
	PlaneFromPoints( &P0, &P1 , &P2 , &g_FOVTopPlane );

//	Calculate bottom plane using 3 points
	P0.x = 0.0f;
	P0.y = 0.0f;
	P0.z = 0.0f;
	P1.x = BackClip * ( (float)tan( Angle * 0.5f ) / Aspect);
	P1.y = -BackClip * ( (float)tan( Angle * 0.5f ) );
	P1.z = BackClip;
	P2.x = -P1.x;
	P2.y = P1.y;
	P2.z = P1.z;
	PlaneFromPoints( &P0, &P1 , &P2 , &g_FOVBottomPlane );
}


BOOL	MeshFOVCheck(VECTOR *pBSpherePos , float BSphereRadius,VECTOR *pViewPos)
{
	float	Dist;

//	Transform Z into view space
	pViewPos->z = g_ViewTransform._13 * pBSpherePos->x +
		g_ViewTransform._23 * pBSpherePos->y +
		g_ViewTransform._33 * pBSpherePos->z + g_ViewTransform._43;

//	Behind front clip plane?
	if( ( pViewPos->z + BSphereRadius ) < g_FOVFrontClip )
		return FALSE;

//	Beyond the back clip plane?
	if( ( pViewPos->z - BSphereRadius ) > g_FOVBackClip )
		return FALSE;

//	Transform X into view space
	pViewPos->x = g_ViewTransform._11 * pBSpherePos->x +
		g_ViewTransform._21 * pBSpherePos->y +
		g_ViewTransform._31 * pBSpherePos->z + g_ViewTransform._41;

//	Test against left clip plane
	Dist = ( pViewPos->x * g_FOVLeftPlane.a ) + ( pViewPos->z * g_FOVLeftPlane.c );
	if( Dist > BSphereRadius )
		return FALSE;

//	Test against right clip plane
	Dist = ( pViewPos->x * g_FOVRightPlane.a ) + ( pViewPos->z * g_FOVRightPlane.c );
	if( Dist > BSphereRadius )
		return FALSE;

//	Transform Y into view space
	pViewPos->y = g_ViewTransform._12 * pBSpherePos->x +
		g_ViewTransform._22 * pBSpherePos->y +
		g_ViewTransform._32 * pBSpherePos->z + g_ViewTransform._42;

//	Test against top clip plane
	Dist = ( pViewPos->y * g_FOVTopPlane.b ) + ( pViewPos->z * g_FOVTopPlane.c );
	if( Dist > BSphereRadius )
		return FALSE;

//	Test against bottom plane
	Dist = ( pViewPos->y * g_FOVBottomPlane.b ) + ( pViewPos->z * g_FOVBottomPlane.c);
	if( Dist > BSphereRadius )
		return FALSE;

//	Mesh is inside the field of view
	return TRUE;
}

