/* Copyright (C) Gabor Nagy, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gabor Nagy, 2000"
 */
/*==============================================================================*/
/* Real-time cast shadows							*/
/*										*/
/* - Shadow map generation							*/
/* - Shadow-matix generation							*/
/*										*/
/* AUTHOR:	Gabor Nagy, Sony Computer Entertainment America			*/
/* DATE:	2000-May-02 03:15:35						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "geo.h"
#include "shadow.h"

// Shadow map
//
extern unsigned long*	Sh_ShadowMapImage;
extern int		Sh_ShadowMapXSize,Sh_ShadowMapYSize;


#define ABS(x)	(((x<0)?(-x):(x)))

/*==============================*/
/* Create the shadow map	*/
/*==============================*/
void Sh_CreateShadowMap(GLuint LShadowMapTexId, int LMapXSize, int LMapYSize, E3dMesh* LBlockerMesh, E3dMatrix LBlockerMatrix)
{
 E3dVertexNode*	LVertexNode;
 E3dPolygon*	LPolygon;
 E3dPolyGroup*	LPolyGroup;
 unsigned long	LC,LN,LPCnt,LPNum;
 E3dMatrix	LTmpMatrix;


 glViewport(0, 0, LMapXSize, LMapYSize);

 glEnable(GL_SCISSOR_TEST);
 glScissor(0, 0, LMapXSize, LMapYSize);

// We will make a dark grey on white shadow-map,
// so clear the buffer with white
//
 glClearColor(1.0, 1.0, 1.0, 0.0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


// Save OpenGL's old PROJECTION matrix
//
 glMatrixMode(GL_PROJECTION);

 glGetFloatv(GL_MODELVIEW_MATRIX,LTmpMatrix);

// Clear OpenGL's PROJECTION matrix
//
 glLoadIdentity();

// Load BlockerLocalToShadowMap matrix
//
 glMatrixMode(GL_MODELVIEW);

 glPushMatrix();
 glLoadMatrixf(LBlockerMatrix);

 glDisable(GL_LIGHTING);
 glDisable(GL_TEXTURE_2D);

// We will draw the blocker object on
// the shadow-map with this color
//
 glColor3f(0.5f, 0.5f, 0.5f);

  LN=LBlockerMesh->NumOfPolyGroups;
  for(LC=0;LC<LN;LC++)
  {
   LPolyGroup=LBlockerMesh->PolyGroups[LC];

   LPNum=LPolyGroup->NumOfPolygons;
   LPolygon=LPolyGroup->Polygons;

// Start drawing shadow-map
//
   glBegin(GL_TRIANGLES);
    for(LPCnt=0;LPCnt<LPNum;LPCnt++,LPolygon++)
    {
     LVertexNode=LPolygon->VertexNodes;
     glVertex3fv( LVertexNode->GLVertex );LVertexNode++;
     glVertex3fv( LVertexNode->GLVertex );LVertexNode++;
     glVertex3fv( LVertexNode->GLVertex );
    }
   glEnd();
  }

 glPopMatrix();

// Restore OpenGL's PROJECTION matrix
//
 glMatrixMode(GL_PROJECTION);
 glLoadMatrixf(LTmpMatrix);

 glDisable(GL_SCISSOR_TEST);


// Make the image we just rendered (the shadow-map) an OpenGL texture
//
 if(Sh_ShadowMapImage!=NULL)
 {
   glReadBuffer(GL_BACK);
   glReadPixels(0, 0, LMapXSize, LMapYSize, GL_RGBA, GL_UNSIGNED_BYTE, Sh_ShadowMapImage);

// Send the shadow map to OpenGL
//
   glBindTexture(GL_TEXTURE_2D, LShadowMapTexId);
   glTexImage2D(GL_TEXTURE_2D, 0, 4, LMapXSize, LMapYSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, Sh_ShadowMapImage );
 }
}


/*======================================================================*/
/* Compute the shadow projection matrices for a given Light and Model	*/
/*======================================================================*/
void E3d_ShadowMatrix(E3dLight* LLight,E3dModel* LModel,E3dMatrix LBlockerMatrix,E3dMatrix LReceiverUVMatrix)
{
 unsigned long		LGmNum,LVn,LVC;
 E3dType		Mx,My,Mz,LPlightX,LPlightY,LPlightZ,
			LRX,LRY,LRZ,			// "Right" vector
			LUPX,LUPY,LUPZ,			// "Up" vector
			LMDVX,LMDVY,LMDVZ,		// Mean Direction Vector
			LVF,LX,LY,LZ,LXMax,LYMax,LXProj,LYProj,
			LZNear,LZFar;
 E3dMesh*		LMesh;
 E3dPolyGroup**		LPolyGroups;
 E3dVertex*		LVertex;
 E3dMatrix		LBlockerLocalToWorldMatrix,LTmpMatrix,LViewMatrix;

 LPlightX=LLight->Position.X;
 LPlightY=LLight->Position.Y;
 LPlightZ=LLight->Position.Z;


 E3d_MatrixCopy(LModel->LocalToWorldMatrix,LBlockerLocalToWorldMatrix);

 LGmNum=LModel->NumOfGeometries;LMesh=(E3dMesh*)(LModel->Geometry);


// Reset Mean Direction Vector
//
 LMDVX=LMDVY=LMDVZ=0.0;

 if(LMesh->NumOfPolyGroups==0) return;
 LPolyGroups=LMesh->PolyGroups;


 LVertex=LMesh->Vertices;

// Average vertex-to-light vectors
//
 LVn=LMesh->NumOfVertices;
 LMDVX=LPlightX*LVn;
 LMDVY=LPlightY*LVn;
 LMDVZ=LPlightZ*LVn;
 for(LVC=0;LVC<LVn;LVC++,LVertex++)
 {
  Mx=LVertex->X;My=LVertex->Y;Mz=LVertex->Z;
  E3dM_MatrixTransform3x4(LBlockerLocalToWorldMatrix,LX,LY,LZ);

  LMDVX-=LX;
  LMDVY-=LY;
  LMDVZ-=LZ;
 }

// Normalize Mean Direction Vector (MDV)
//
 LVF=sqrt(LMDVX*LMDVX+LMDVY*LMDVY+LMDVZ*LMDVZ);LVF=1.0/LVF;

 LMDVX*=LVF;
 LMDVY*=LVF;
 LMDVZ*=LVF;

// Get smallest component of MDV (Z axis of the light coordinate system), make a vector with that
// component as 1, the others 0 (L)
// The RIGHT vector (X axis of the camera transform) will be: LRV = LMDV x L
// The UP-vector (Y axis of the camera transform) will be: LUPV = LMDV x LRV
//
 if(LMDVX<=LMDVY)
 {
  if(LMDVZ<=LMDVX) { LX=0.0;LY=0.0;LZ=1.0; }	// LMDVZ is smallest
  else { LX=1.0;LY=0.0;LZ=0.0; }		// LMDVX is smallest
 }
 else
 {
  if(LMDVZ<=LMDVY) { LX=0.0;LY=0.0;LZ=1.0; }	// LMDVZ is smallest
  else { LX=0.0;LY=1.0;LZ=0.0; }		// LMDVX is smallest
 }

// LR = LMDV x L
//
 E3dM_VectorCrossProduct(LMDVX,LMDVY,LMDVZ,LX,LY,LZ,LRX,LRY,LRZ);

// Normalize Right vector
//
 LVF=sqrt(LRX*LRX+LRY*LRY+LRZ*LRZ);LVF=1.0/LVF;
 LRX*=LVF;LRY*=LVF;LRZ*=LVF;

// LUP = LR x LMDV
//
 E3dM_VectorCrossProduct(LMDVX,LMDVY,LMDVZ,LRX,LRY,LRZ,LUPX,LUPY,LUPZ);


// Build view-transformation matrix
//
 LViewMatrix[M00]=LRX;LViewMatrix[M01]=LUPX;LViewMatrix[M02]=LMDVX;LViewMatrix[M03]=0.0;
 LViewMatrix[M10]=LRY;LViewMatrix[M11]=LUPY;LViewMatrix[M12]=LMDVY;LViewMatrix[M13]=0.0;
 LViewMatrix[M20]=LRZ;LViewMatrix[M21]=LUPZ;LViewMatrix[M22]=LMDVZ;LViewMatrix[M23]=0.0;
 LViewMatrix[M30]=0.0;LViewMatrix[M31]=0.0;LViewMatrix[M32]=0.0;LViewMatrix[M33]=1.0;
 E3d_MatrixTranslate(LViewMatrix,-LPlightX,-LPlightY,-LPlightZ);

 E3d_MatrixCopy(LViewMatrix,LTmpMatrix);
 E3d_MatrixMult(LBlockerLocalToWorldMatrix,LTmpMatrix);
 

// Find the horizontal and vertical angular spreads to find out FOV and aspect ratio
//
 LVertex=LMesh->Vertices;
 LXMax=LYMax=0.0;
 for(LVC=0,LVn=LMesh->NumOfVertices;LVC<LVn;LVC++,LVertex++)
 {
  Mx=LVertex->X;My=LVertex->Y;Mz=LVertex->Z;
  E3dM_MatrixTransform3x4(LTmpMatrix,LX,LY,LZ);
  LZ=1.0/LZ;

  LX*=LZ;LY*=LZ;
  if(ABS(LX)>LXMax) LXMax=ABS(LX);
  if(ABS(LY)>LYMax) LYMax=ABS(LY);
 }

 LZNear=0.1;LZFar=1000.0;
// Set up the perspective-projection matrix
//
//    f = cotangent(Field-of-view) = 1.0/LYMax
//    f/aspect=1.0/LXMax
//
//	| f/aspect   0	     0.0	0.0   |
//	|				      |
//	|				      |
//	|				      |
//	|    0       f	     0.0	0.0   |
//	|				      |
//	|				      |
//	|                 Zfar+Znear	      |
//	|    0       0    ----------   -1.0   |
//	|	          Znear-Zfar	      |
//	|				      |
//	|	         2*Zfar*Znear	      |
//	|    0       0   ------------   0.0   |
//	|                 Znear-Zfar	      |

// Leave a pixel or two around the edges of the
// shadow map for texture clamping
//
 LXProj=(0.995-1.0/(float)Sh_ShadowMapXSize)/LXMax;
 LYProj=(0.995-1.0/(float)Sh_ShadowMapYSize)/LYMax;
 LBlockerMatrix[M00]=LXProj;LBlockerMatrix[M01]=   0.0;LBlockerMatrix[M02]=0.0;LBlockerMatrix[M03]=0.0;
 LBlockerMatrix[M10]=   0.0;LBlockerMatrix[M11]=LYProj;LBlockerMatrix[M12]=0.0;LBlockerMatrix[M13]=0.0;
 LBlockerMatrix[M20]=   0.0;LBlockerMatrix[M21]=   0.0;LBlockerMatrix[M22]=(LZFar+LZNear)/(LZNear-LZFar);LBlockerMatrix[M23]=-1.0;
 LBlockerMatrix[M30]=   0.0;LBlockerMatrix[M31]=   0.0;LBlockerMatrix[M32]=2.0*LZFar*LZNear/(LZNear-LZFar);LBlockerMatrix[M33]=0.0;

// ViewMatrix = Projection * ViewTransformMatrix * LocalToWorld
//
 E3d_MatrixMult(LViewMatrix,LBlockerMatrix);

 E3d_MatrixMult(LBlockerLocalToWorldMatrix,LBlockerMatrix);


// Projection matrix for computing UVs on the receiver object
//
 LReceiverUVMatrix[M00]=LXProj*0.5;LReceiverUVMatrix[M01]=       0.0;LReceiverUVMatrix[M02]=0.0;LReceiverUVMatrix[M03]=0.0;
 LReceiverUVMatrix[M10]=       0.0;LReceiverUVMatrix[M11]=LYProj*0.5;LReceiverUVMatrix[M12]=0.0;LReceiverUVMatrix[M13]=0.0;
 LReceiverUVMatrix[M20]=      -0.5;LReceiverUVMatrix[M21]=      -0.5;LReceiverUVMatrix[M22]=(LZFar+LZNear)/(LZNear-LZFar);LReceiverUVMatrix[M23]=-1.0;
 LReceiverUVMatrix[M30]=       0.0;LReceiverUVMatrix[M31]=       0.0;LReceiverUVMatrix[M32]=2.0*LZFar*LZNear/(LZNear-LZFar);LReceiverUVMatrix[M33]=0.0;

 E3d_MatrixMult(LViewMatrix,LReceiverUVMatrix);
}
