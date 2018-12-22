/* Copyright (C) David Paull, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) David Paull, 2000"
 */
// VectorCamera.cpp
// author : David Paull
// email : david@tanzanite.to
// desc : This is a minature opengl version of the 
// original, more complete, D3D version. 
// The latest version of both the D3D, and 
// OpenGl vesion can be found at www.tanzanite.to

#include <math.h>
#include <memory.h>
#include "glut.h"
#include "vectorcamera.h"
#include "mtxlib.h"
#pragma warning ( disable : 4514)//unreferenced inline function has been removed


extern unsigned long gpgtexture[128*128];
const unsigned int	WindowWidth  = 640;//292;
const unsigned int	WindowHeight = 480;//254;

// Vector Camera Data
unsigned char pOldDexToNewDEX[MAX_VERT];
unsigned char pVisibleTri[MAX_TRI];
unsigned char pVisibleVert[MAX_VERT];
float fScreenWidth;
float fScreenHeight;
float fAspectRatio;
float SaveHfrac;
float SaveVfrac;
float oo2hFrac;
float oo2vFrac;
long WidthScale;
long HeightScale;
vector4 LocalPlanes[5];
vector4 wClipPlane[5];
vector4 lClipPlane[5];
vector4 U;
vector4 V;
vector4 N;
vector4 P;
vector4 lS;
vector4 lT;
vector4 lDir;
vector4 lUp;
vector4 lRight;
vector4 lPos;
float Length;
vector4      pTLV[MAX_VERT];
unsigned int cTLV = 0;
unsigned int pTLT[MAX_TRI];
unsigned int cTLT = 0;


void InitVectorCamera()
{
	fScreenWidth  = (float)WindowWidth;
	fScreenHeight = (float)WindowHeight;
	if(fScreenHeight>fScreenWidth)
	{
		//////////////////////////////////
		//// Tall And Skinny Viewport ////
		//////////////////////////////////
		fAspectRatio = fScreenWidth/fScreenHeight;
		SaveHfrac =  tan(FOV*0.5*fAspectRatio);
		SaveVfrac =  tan(FOV*0.5);
	}
	else
	{
		//////////////////////////////
		//// Normal Wide Viewport ////
		//////////////////////////////
		fAspectRatio = fScreenHeight/fScreenWidth;
		SaveHfrac =  tan(FOV*0.5);
		SaveVfrac =  tan(FOV*0.5*fAspectRatio);
	}

	oo2hFrac  = 1.0f/(SaveHfrac*2.0f);
	oo2vFrac  = 1.0f/(SaveVfrac*2.0f);
	// Initialize Local Version Of Camera
	//.........................................................................
	LocalPlanes[0].x = 0.0;
	LocalPlanes[0].y = 0.0;
	LocalPlanes[0].z = 1.0;
	//.........................................................................
	LocalPlanes[1].x = 1.0;
	LocalPlanes[1].y = 0.0;
	LocalPlanes[1].z = SaveHfrac;
	MacroNormalize(Length,LocalPlanes[1].x,LocalPlanes[1].y,LocalPlanes[1].z);
	//.........................................................................
	LocalPlanes[2].x = -1.0;
	LocalPlanes[2].y = 0.0;
	LocalPlanes[2].z = SaveHfrac;
	MacroNormalize(Length,LocalPlanes[2].x,LocalPlanes[2].y,LocalPlanes[2].z);
	//.........................................................................
	LocalPlanes[3].x = 0.0;
	LocalPlanes[3].y = -1.0;
	LocalPlanes[3].z = SaveVfrac;
	MacroNormalize(Length,LocalPlanes[3].x,LocalPlanes[3].y,LocalPlanes[3].z);
	//.........................................................................
	LocalPlanes[4].x = 0.0;
	LocalPlanes[4].y = 1.0;
	LocalPlanes[4].z = SaveVfrac;
	MacroNormalize(Length,LocalPlanes[4].x,LocalPlanes[4].y,LocalPlanes[4].z);
	//.........................................................................
}
//Draw the Spinner using the Vector Camera
void vcDisplayMesh(tMesh& Mesh,matrix44& RotMatrix,unsigned int mTextureID)
{
	long V1,V2,V3;
	vector4 *pLocalV = NULL;
	unsigned int      *pLocalT = NULL;
	vector4   *pLocalN = NULL;
	float     *pLocalD = NULL;
	vector4 bsWorldPos;
	matrix44 WtoL;
	PolyGon pIn;
	PolyGon pOut;
	long cTri;
	long cVert;
	float NdotP;
	float NdotR;
	vector4 lScrnPos;
	float t;
	vector4 lRay;


	CalculateFrustumPlanes(wClipPlane,U,V,N,P);
	//ToDo: Transform Bounding Sphere Center, and if(WorldBoundingSphereInFrustum)
	WtoL = IdentityMatrix44();
	//Invert LocalToWorld into a WorldToLocal matrix
	MatrixInverse(WtoL,RotMatrix);

	// Inverse Transform Camera.
	lPos.x   = (WtoL[0][0]*P.x) + (WtoL[1][0]*P.y) + (WtoL[2][0]*P.z) + (WtoL[3][0]);
	lPos.y   = (WtoL[0][1]*P.x) + (WtoL[1][1]*P.y) + (WtoL[2][1]*P.z) + (WtoL[3][1]);
	lPos.z   = (WtoL[0][2]*P.x) + (WtoL[1][2]*P.y) + (WtoL[2][2]*P.z) + (WtoL[3][2]);

	lDir.x   = (WtoL[0][0]*N.x) + (WtoL[1][0]*N.y) + (WtoL[2][0]*N.z);
	lDir.y   = (WtoL[0][1]*N.x) + (WtoL[1][1]*N.y) + (WtoL[2][1]*N.z);
	lDir.z   = (WtoL[0][2]*N.x) + (WtoL[1][2]*N.y) + (WtoL[2][2]*N.z);

	lRight.x = (WtoL[0][0]*U.x) + (WtoL[1][0]*U.y) + (WtoL[2][0]*U.z);
	lRight.y = (WtoL[0][1]*U.x) + (WtoL[1][1]*U.y) + (WtoL[2][1]*U.z);
	lRight.z = (WtoL[0][2]*U.x) + (WtoL[1][2]*U.y) + (WtoL[2][2]*U.z);

	lUp.x    = (WtoL[0][0]*V.x) + (WtoL[1][0]*V.y) + (WtoL[2][0]*V.z);
	lUp.y    = (WtoL[0][1]*V.x) + (WtoL[1][1]*V.y) + (WtoL[2][1]*V.z);
	lUp.z    = (WtoL[0][2]*V.x) + (WtoL[1][2]*V.y) + (WtoL[2][2]*V.z);
	
	//ToDo : if(obj dot ldir < lpos dot ldir) Object is behind camera. Early Out here
	//ToDo : Transform All Light pos Into Local
	//Calc local version of camera's viewing pyramid, the frustum.
	CalculateFrustumPlanes(lClipPlane,lRight,lUp,lDir,lPos);
	//Calc Screen S, and T vectors.
	CalculateScreenVectors(lS,lT,lRight,lUp,lPos,SaveHfrac,SaveVfrac);			

	pLocalV = Mesh.pV;//Verts
	pLocalT = Mesh.pT;//Tris
	pLocalN = Mesh.pN;//Plane Normals
	pLocalD = Mesh.pD;//Plane Distance

	memset(pVisibleVert,0,(MAX_VERT*sizeof(unsigned char)));
	memset(pVisibleTri, 0,(MAX_TRI*sizeof(unsigned char)));

	////////////////////////////
	//// Flag Visible Tris  ////
	////////////////////////////

	for(cTri=0;cTri<Mesh.cT;cTri++)
	{
		NdotP = DotProduct3(pLocalN[cTri],lPos);
		MacroDot(NdotP,pLocalN[cTri].x,pLocalN[cTri].y,pLocalN[cTri].z,lPos.x,lPos.y,lPos.z);
		if(1)//NdotP > pLocalD[cTri])
		{//for each facing tri, flag all of its verts as visible.
			pVisibleTri[cTri] = 1;
			V1 = pLocalT[(cTri*3  )];
			V2 = pLocalT[(cTri*3+1)];
			V3 = pLocalT[(cTri*3+2)];
			pVisibleVert[V1] = 1;
			pVisibleVert[V2] = 1;
			pVisibleVert[V3] = 1;
		}
	}

	/////////////////////////////////////////////////
	//// For Every Vert, Compute Local To Screen ////
	/////////////////////////////////////////////////

	cTLV = 0;
	for(cVert=0;cVert<Mesh.cV;cVert++)
	{
		if(pVisibleVert[cVert])
		{
			// The camera is in local space with the object. 
			// Their spatial orientation, and position are the 
			// same as if they both were in world space.
			// All rays, and vectors, and positions are in local coord.
			//create ray from camera's local space pos to the vertex's local space pos
			lRay.x = pLocalV[cVert].x - lPos.x;
			lRay.y = pLocalV[cVert].y - lPos.y;
			lRay.z = pLocalV[cVert].z - lPos.z;
			//
			//Project ray onto Dir vector. ie, Z depth
			MacroDot(NdotR,lRay.x,lRay.y,lRay.z,lDir.x,lDir.y,lDir.z);
			//The near plane is at 1.0
			t          = 1.0f/NdotR;
			//find intersection point for ray, and near plane
			lScrnPos.x = lPos.x + lRay.x * t;
			lScrnPos.y = lPos.y + lRay.y * t;
			lScrnPos.z = lPos.z + lRay.z * t;
			//Find distance of i-point along the S vector.
			MacroDot(Length,1.0,0.0,0.0,lS.x,lS.y,lS.z);
			MacroDot(Length,lScrnPos.x,lScrnPos.y,lScrnPos.z,lS.x,lS.y,lS.z);
			Length -= lS.w;//(w is plane equation d)
			//Scale first by oo2hFrac which is the half width of the near plane.
			//Then scale by the width of the screen.
			pTLV[cTLV].x = Length*oo2hFrac*(fScreenWidth-1.0)+0.5;
			//Find distance of i-point along the T vector.
			MacroDot(Length,lScrnPos.x,lScrnPos.y,lScrnPos.z,lT.x,lT.y,lT.z);
			Length -= lT.w;//(w is plane equation d)
			//Scale first by oo2vFrac which is the half height of the near plane.
			//Then scale by the height of the screen.
			pTLV[cTLV].y = Length*oo2vFrac*(fScreenHeight-1.0)+0.5;
			//perspective correct textures will need z.
			pTLV[cTLV].z = NdotR*(1.0/512.0);

			pOldDexToNewDEX[cVert]	= cTLV;
			cTLV++;
		}
	}

	// 3d clipping goes here. It has been removed to keep this sample small. 
	// The full D3D version is available at www.tanzanite.to


	glPushMatrix();

	//in D3D you can bypass transformation by using D3DTLVERTEX.
	// we will fake it here by loading an identity matrix.
	// ToDo : turn off opengl transformation.
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glBegin(GL_QUADS);

	////////////////////////////////////////
	glColor4ub(0,0xff,  0, 0xff);
	glTexCoord2f(1.0F, 0.0F);
	glVertex3f(pTLV[0].x,pTLV[0].y,pTLV[0].z);
	////////////////////////////////////////
	glColor4ub(0x00, 0, 0xff, 0xff);
	glTexCoord2f(0.0F, 0.0F);
	glVertex3f(pTLV[1].x,pTLV[1].y,pTLV[1].z);
	////////////////////////////////////////
	glColor4ub(0xff,0, 0, 0xff);
	glTexCoord2f(0.0F, 1.0F);
	glVertex3f(pTLV[2].x,pTLV[2].y,pTLV[2].z);
	////////////////////////////////////////
	glColor4ub(0xff, 0, 0xff, 0xff);
	glTexCoord2f(1.0F, 1.0F);
	glVertex3f(pTLV[3].x,pTLV[3].y,pTLV[3].z);
	////////////////////////////////////////

	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Pop the rotation matrix from the MODELVIEW stack
	glPopMatrix();

}
void CalculateFrustumPlanes(vector4 ClipPlane[],
							vector4 &Right,
							vector4 &Up,
							vector4 &Dir,
							vector4 &Pos)
{
	vector4 temp;
	///////////////////////////////////
	//// World Space : Front Plane ////
	///////////////////////////////////
	ClipPlane[0] = Dir;
	temp = Pos+Dir;
	ClipPlane[0].w = DotProduct3(ClipPlane[0],temp);
	//////////////////////////////////
	//// World Space : Left Plane ////
	//////////////////////////////////
	ClipPlane[1].x = (LocalPlanes[1].x*Right.x) + (LocalPlanes[1].y*Up.x) + (LocalPlanes[1].z*Dir.x);
	ClipPlane[1].y = (LocalPlanes[1].x*Right.y) + (LocalPlanes[1].y*Up.y) + (LocalPlanes[1].z*Dir.y);
	ClipPlane[1].z = (LocalPlanes[1].x*Right.z) + (LocalPlanes[1].y*Up.z) + (LocalPlanes[1].z*Dir.z);
	ClipPlane[1].w = DotProduct3(ClipPlane[1],Pos);
	///////////////////////////////////
	//// World Space : Right Plane ////
	///////////////////////////////////
	ClipPlane[2].x = (LocalPlanes[2].x*Right.x) + (LocalPlanes[2].y*Up.x) + (LocalPlanes[2].z*Dir.x);
	ClipPlane[2].y = (LocalPlanes[2].x*Right.y) + (LocalPlanes[2].y*Up.y) + (LocalPlanes[2].z*Dir.y);
	ClipPlane[2].z = (LocalPlanes[2].x*Right.z) + (LocalPlanes[2].y*Up.z) + (LocalPlanes[2].z*Dir.z);
	ClipPlane[2].w = DotProduct3(ClipPlane[2],Pos);
	/////////////////////////////////
	//// World Space : Top Plane ////
	/////////////////////////////////
	ClipPlane[3].x = (LocalPlanes[3].x*Right.x) + (LocalPlanes[3].y*Up.x) + (LocalPlanes[3].z*Dir.x);
	ClipPlane[3].y = (LocalPlanes[3].x*Right.y) + (LocalPlanes[3].y*Up.y) + (LocalPlanes[3].z*Dir.y);
	ClipPlane[3].z = (LocalPlanes[3].x*Right.z) + (LocalPlanes[3].y*Up.z) + (LocalPlanes[3].z*Dir.z);
	ClipPlane[3].w = DotProduct3(ClipPlane[3],Pos);
	////////////////////////////////////
	//// World Space : Bottom Plane ////
	////////////////////////////////////
	ClipPlane[4].x = (LocalPlanes[4].x*Right.x) + (LocalPlanes[4].y*Up.x) + (LocalPlanes[4].z*Dir.x);
	ClipPlane[4].y = (LocalPlanes[4].x*Right.y) + (LocalPlanes[4].y*Up.y) + (LocalPlanes[4].z*Dir.y);
	ClipPlane[4].z = (LocalPlanes[4].x*Right.z) + (LocalPlanes[4].y*Up.z) + (LocalPlanes[4].z*Dir.z);
	ClipPlane[4].w = DotProduct3(ClipPlane[4],Pos);
}
//...........................................................................................................................
void CalculateScreenVectors(vector4& ScreenS,
							vector4& ScreenT,
							vector4& Right,
							vector4& Up,
							vector4& Pos,
							float hFrac,
							float vFrac)
{
	//// Local Coordinates : The Cameras Screen at z=1. S,T Vectors represent X,Y pixels

	ScreenS.x = Right.x;
	ScreenS.y = Right.y;
	ScreenS.z = Right.z;

	vector4 temp;
	temp = Pos - (hFrac*Right);
	ScreenS.w = DotProduct3(ScreenS,temp);

	ScreenT.x = Up.x;
	ScreenT.y = Up.y;
	ScreenT.z = Up.z;

	temp = Pos - (vFrac*Up);
	ScreenT.w = DotProduct3(ScreenT,temp);
}

void MatrixInverse(matrix44 &dst,matrix44 &src)
{
  //////////////////////////////////////////////
  //// Calculate Transpose Of Source Matrix ////
  //////////////////////////////////////////////
 	
  dst[0][0] = src[0][0];
  dst[0][1] = src[1][0];
  dst[0][2] = src[2][0];

  dst[1][0] = src[0][1];
  dst[1][1] = src[1][1];
  dst[1][2] = src[2][1];

  dst[2][0] = src[0][2];
  dst[2][1] = src[1][2];
  dst[2][2] = src[2][2];

  //////////////////////////////////////////////////////////////////////
  //// Transform Negated Source Position Into New Coordinate System ////
  //////////////////////////////////////////////////////////////////////

  dst[3][0] = (-src[3][0]*dst[0][0])+(-src[3][1]*dst[1][0])+(-src[3][2]*dst[2][0]);
  dst[3][1] = (-src[3][0]*dst[0][1])+(-src[3][1]*dst[1][1])+(-src[3][2]*dst[2][1]);
  dst[3][2] = (-src[3][0]*dst[0][2])+(-src[3][1]*dst[1][2])+(-src[3][2]*dst[2][2]);
}

void vcLookAt(	float CameraPosX,
				float CameraPosY,
				float CameraPosZ, 
				float LookatPosX,
				float LookatPosY,
				float LookatPosZ, 
				float UpX,
				float UpY,
				float UpZ)
{
	// see www.tanzanite.to for full featured d3d version
	//    as well as updates to the OpenGl version.
	P.x = CameraPosX;
	P.y = CameraPosY;
	P.z = CameraPosZ;

	V.x = UpX;
	V.y = UpY;
	V.z = UpZ; 

	N.x = LookatPosX-P.x;
	N.y = LookatPosY-P.y;
	N.z = LookatPosZ-P.z; 

	N = Normalized3(N);
	U = CrossProduct3(V,N);
	U = Normalized3(U);
	U = CrossProduct3(V,N);
	V = CrossProduct3(N,U);
	V = Normalized3(V);
}
