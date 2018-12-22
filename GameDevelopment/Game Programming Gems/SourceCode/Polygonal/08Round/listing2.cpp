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
#include "listing2.h"

#ifndef _WIN32
typedef unsigned int BOOL;
#define TRUE 1
#define FALSE 0
#endif

typedef struct _OCCLUSION
{
	VECTOR	P0;
	VECTOR	P1;
	VECTOR	P2;
	VECTOR	P3;
	float	MinZ;
	PLANE	FrontPlane;
	PLANE	FirstPlane;
	PLANE	SecondPlane;
	PLANE	ThirdPlane;
	PLANE	FourthPlane;
}OCCLUSION;

void	VectorMatrixMultiply3x4(VECTOR *pNewVector , VECTOR *pVector , MATRIX *pMatrix)
{
}

void	SetupOcclusion(OCCLUSION *pOcclusion , MATRIX *pViewTransform)
{
	VECTOR	P0 , P1 , P2 , P3 , Camera;

//	Transform points form world space to view space
	VectorMatrixMultiply3x4( &P0 , &pOcclusion->P0 , pViewTransform);
	VectorMatrixMultiply3x4( &P1 , &pOcclusion->P1 , pViewTransform);
	VectorMatrixMultiply3x4( &P2 , &pOcclusion->P2 , pViewTransform);
	VectorMatrixMultiply3x4( &P3 , &pOcclusion->P3 , pViewTransform);

	pOcclusion->MinZ = P0.z;
	if( P1.z < pOcclusion->MinZ)
		pOcclusion->MinZ = P1.z;
	if( P2.z < pOcclusion->MinZ)
		pOcclusion->MinZ = P2.z;
	if( P3.z < pOcclusion->MinZ)
		pOcclusion->MinZ = P3.z;

//	The camera position in view space is 0,0,0
	Camera.x=0.0f;
	Camera.y=0.0f;
	Camera.z=0.0f;

//	Create front plane from first three points
	PlaneFromPoints(&P0 , &P1 , &P2 , &pOcclusion->FrontPlane);

//	Test the D co-effecient to find which way the occlusion faces
	if(pOcclusion->FrontPlane.d > 0.0f)
	{
		PlaneFromPoints( &Camera , &P0 , &P1 , &pOcclusion->FirstPlane);
		PlaneFromPoints( &Camera , &P1 , &P2 , &pOcclusion->SecondPlane);
		PlaneFromPoints( &Camera , &P2 , &P3 , &pOcclusion->ThirdPlane);
		PlaneFromPoints( &Camera , &P3 , &P0 , &pOcclusion->FourthPlane);
	}
	else
	{
		PlaneFromPoints( &P2 , &P1 , &P0 , &pOcclusion->FrontPlane);
		PlaneFromPoints( &Camera , &P1 , &P0 , &pOcclusion->FirstPlane);
		PlaneFromPoints( &Camera , &P2 , &P1 , &pOcclusion->SecondPlane);
		PlaneFromPoints( &Camera , &P3 , &P2 , &pOcclusion->ThirdPlane);
		PlaneFromPoints( &Camera , &P0 , &P3 , &pOcclusion->FourthPlane);
	}
}

BOOL	TestIfOccluded(OCCLUSION *pOcclusion , VECTOR *pViewPos , float BSphereRadius)
{
	float	MinZ;

	MinZ = pViewPos->z - BSphereRadius;
	if( pOcclusion->MinZ < MinZ )
		return FALSE;
	if( ( ( pViewPos->x * pOcclusion->FrontPlane.a) + 
		(pViewPos->y * pOcclusion->FrontPlane.b) +
		(pViewPos->z * pOcclusion->FrontPlane.c) + pOcclusion->FrontPlane.d) > 
		BSphereRadius )
		return FALSE;
	if( ( ( pViewPos->x * pOcclusion->FirstPlane.a) + 
		(pViewPos->y * pOcclusion->FirstPlane.b) +
		(pViewPos->z * pOcclusion->FirstPlane.c) + pOcclusion->FirstPlane.d) >
		BSphereRadius )
		return FALSE;
	if( ( ( pViewPos->x * pOcclusion->SecondPlane.a) + 
		(pViewPos->y * pOcclusion->SecondPlane.b) +
		(pViewPos->z * pOcclusion->SecondPlane.c) + pOcclusion->FirstPlane.d) > 
		BSphereRadius )
		return FALSE;
	if( ( ( pViewPos->x * pOcclusion->ThirdPlane.a) + 
		(pViewPos->y * pOcclusion->ThirdPlane.b) +
		(pViewPos->z * pOcclusion->ThirdPlane.c) + pOcclusion->FirstPlane.d) >
		BSphereRadius )
		return FALSE;
	if( ( ( pViewPos->x * pOcclusion->FourthPlane.a) + 
		(pViewPos->y * pOcclusion->FourthPlane.b) +
		(pViewPos->z * pOcclusion->FourthPlane.c) + pOcclusion->FirstPlane.d) > 
		BSphereRadius )
		return FALSE;
	return TRUE;
}

