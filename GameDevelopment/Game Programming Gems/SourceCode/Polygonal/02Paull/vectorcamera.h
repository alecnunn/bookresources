/* Copyright (C) David Paull, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) David Paull, 2000"
 */
#include "mtxlib.h"

#define FOV 1.5707963267948966192313216916398
#define MacroDot(Length,Ax,Ay,Az,Bx,By,Bz)\
			(Length = (Ax*Bx+Ay*By+Az*Bz)) 
//...............................................................................
#define MacroNormalize(Length,Ax,Ay,Az)\
			((Length=(1.0/(sqrt(Ax*Ax+Ay*Ay+Az*Az)))),\
			(Ax *= Length),\
			(Ay *= Length),\
			(Az *= Length))
//...............................................................................
#define MacroCross(Rx,Ry,Rz,Ax,Ay,Az,Bx,By,Bz)\
			((Rx = Ay*Bz-Az*By),\
			( Ry = Az*Bx-Ax*Bz),\
			( Rz = Ax*By-Ay*Bx))

// This is the texture used by the spinnerVC
#define MAX_VERT 4
#define MAX_TRI 2


typedef struct _PolyGon
{
    long      numverts;
    vector4 verts[MAX_VERT];
}PolyGon;


typedef struct _tMesh
{
	vector4 *pV;//Verts
	unsigned int    *pT;//Tris
	vector4 *pN;//Plane Normals
	float   *pD;//Plane Distance
	long     cV;//Num Verts
	long     cT;//Num Tris
	matrix44 LtoW;
	vector4  bsPos;
	float	   bsRad;
	unsigned int pPlaneIndicies[5];//0-4 tells render which clip plane Bounding sphere is touching
	unsigned int cPlaneIndicies;   //0-4 number of clip planes that the BS is touching
	long       Visible;
}tMesh;




void InitVectorCamera();
void vcDisplayMesh(tMesh& Mesh,matrix44& RotMatrix,unsigned int mTextureID);
void CalculateFrustumPlanes(vector4 ClipPlane[],
							vector4 &Right,
							vector4 &Up,
							vector4 &Dir,
							vector4 &Pos);
void CalculateScreenVectors(vector4& ScreenS,
							vector4& ScreenT,
							vector4& Right,
							vector4& Up,
							vector4& Pos,
							float hFrac,
							float vFrac);
void MatrixInverse(matrix44 &dst,matrix44 &src);
void vcLookAt(	float CameraPosX,
				float CameraPosY,
				float CameraPosZ, 
				float LookatPosX,
				float LookatPosY,
				float LookatPosZ, 
				float UpX,
				float UpY,
				float UpZ);
