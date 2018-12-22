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
/* Simple 3D library for reading/displaying geometry				*/
/*										*/
/* - Display functions								*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-May-02 23:57:53						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#ifdef _WIN32
#include <windows.h>
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "geo.h"

E3dMaterial	E3d_DefaultMaterial;

/*======================================*/
/* Render an object			*/
/*======================================*/
void E3d_DrawModel(E3dModel* LModel, EBool LTextureOn, E3dLight* LLightForShadow, EBool LRemoveBackfaceShadows)
{
 E3dMaterial*	LMaterial;
 E3dMesh*	LMesh;
 E3dPolyGroup*	LPolyGroup;
 E3dPolygon*	LPolygon;
 E3dVertexNode*	LVertexNode;
 unsigned long	LC, LN, LPCnt, LPNum;
 float		Mx,My,Mz, LXF, LYF, LZF, LWF, LVXF, LVYF, LVZF;
 float		LUV0[3], LUV1[3], LUV2[3];
 E3dMatrix	LLocalToWorldMatrix, LLocalToShadowMapSTMatrix;

 if(LModel->Geometry!=NULL)
 {
  glMultMatrixf(LModel->LocalToWorldMatrix);

  E3d_MatrixCopy(LModel->LocalToWorldMatrix, LLocalToWorldMatrix);

  LMesh=(E3dMesh*)(LModel->Geometry);

  LN=LMesh->NumOfPolyGroups;

  for(LC=0;LC<LN;LC++)
  {
   LPolyGroup=LMesh->PolyGroups[LC];

// Set material or flat color for drawing an object
//
   if((LMaterial=LPolyGroup->Material)!=NULL)
   {
    if(LMaterial->Type==E3dMAT_CONSTANT)
    {
     glColor3f(LMaterial->Diffuse[0], LMaterial->Diffuse[1], LMaterial->Diffuse[2]);
    }
    else
    {
     glMaterialfv(GL_FRONT,GL_AMBIENT, LMaterial->Ambient);
     glMaterialfv(GL_FRONT,GL_DIFFUSE, LMaterial->Diffuse);
    }
   }
   else
   {
    glMaterialfv(GL_FRONT,GL_AMBIENT, E3d_DefaultMaterial.Ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE, E3d_DefaultMaterial.Diffuse);
   }

   LPNum=LPolyGroup->NumOfPolygons;
   LPolygon=LPolyGroup->Polygons;

// Is this a shadow-pass?
//
   if(LPolyGroup->ShadowPass)
   {
// Get Local-To-shadow-map-ST matrix
//
    E3d_MatrixCopy(LModel->ShadowMapUVMatrix, LLocalToShadowMapSTMatrix);
    E3d_MatrixMult(LLocalToWorldMatrix, LLocalToShadowMapSTMatrix);

// Receiver object shadow rendering pass
//
    if(LRemoveBackfaceShadows&&(LLightForShadow!=NULL))
    {
// Perform back-face shadow elimination on receiver
//

// We will do the shadow-map coordinate computation manually,
// so we have to set OpenGL's TEXTURE matrix to identity
//
     glMatrixMode(GL_TEXTURE);
     glLoadIdentity();
     glMatrixMode(GL_MODELVIEW);

// Start drawing
//
     glBegin(GL_TRIANGLES);

      for(LPCnt=0;LPCnt<LPNum;LPCnt++,LPolygon++)
      {
       LVertexNode=LPolygon->VertexNodes;

// Transform polygon normal with LLocalToWorldMatrix to determine whether the triangle
// is facing away from the light source
//
       Mx=LPolygon->Normal.X;My=LPolygon->Normal.Y;Mz=LPolygon->Normal.Z;
       E3dM_MatrixTransformVector3x3(LLocalToWorldMatrix, LXF, LYF, LZF);

// Transform first vertex with LLocalToWorldMatrix to determine whether the triangle
// is facing away from the light source
//
       Mx=LVertexNode->GLVertex[0];My=LVertexNode->GLVertex[1];Mz=LVertexNode->GLVertex[2];
       E3dM_MatrixTransform3x4(LLocalToWorldMatrix, LVXF, LVYF, LVZF);

// Get vertex-to-light vector on first vertex
//
       LVXF-=LLightForShadow->Position.X;
       LVYF-=LLightForShadow->Position.Y;
       LVZF-=LLightForShadow->Position.Z;

// Use dot-product of the transformed normal and the vertex-to-light vector
//
       if((LVXF*LXF+LVYF*LYF+LVZF*LZF)>=0.0)
       {
	glNormal3fv( LVertexNode->GLNormal );
	LUV0[0]=-1.0;LUV0[1]=-1.0;
	glTexCoord2fv(LUV0);
	glVertex3fv( LVertexNode->GLVertex );
	LVertexNode++;

	glNormal3fv( LVertexNode->GLNormal );
	glTexCoord2fv(LUV0);
	glVertex3fv( LVertexNode->GLVertex );
	LVertexNode++;

	glNormal3fv( LVertexNode->GLNormal );
	glTexCoord2fv(LUV0);
	glVertex3fv( LVertexNode->GLVertex );
       }
       else
       {
	glNormal3fv( LVertexNode->GLNormal );

// Compute texture coordinates for triangle vertex 0:
// Transform Vertex0 with the Model's ShadowMapUV matrix
//
	Mx=LVertexNode->GLVertex[0];My=LVertexNode->GLVertex[1];Mz=LVertexNode->GLVertex[2];
	E3dM_MatrixTransformHPosition(LLocalToShadowMapSTMatrix);LWF=1.0f/LWF;
	LUV0[0]=LXF*LWF;LUV0[1]=LYF*LWF;
	glTexCoord2fv(LUV0);

	glVertex3fv( LVertexNode->GLVertex );

	LVertexNode++;


	glNormal3fv( LVertexNode->GLNormal );

// Compute texture coordinates for triangle vertex 1:
// Transform Vertex1 with the Model's ShadowMapUV matrix
//
	Mx=LVertexNode->GLVertex[0];My=LVertexNode->GLVertex[1];Mz=LVertexNode->GLVertex[2];
	E3dM_MatrixTransformHPosition(LLocalToShadowMapSTMatrix);LWF=1.0f/LWF;
	LUV1[0]=LXF*LWF;LUV1[1]=LYF*LWF;
	glTexCoord2fv(LUV1);

	glVertex3fv( LVertexNode->GLVertex );

	LVertexNode++;


// Compute texture coordinates for triangle vertex 2:
// Transform Vertex2 with the Model's ShadowMapUV matrix
//
	glNormal3fv( LVertexNode->GLNormal );
	Mx=LVertexNode->GLVertex[0];My=LVertexNode->GLVertex[1];Mz=LVertexNode->GLVertex[2];
	E3dM_MatrixTransformHPosition(LLocalToShadowMapSTMatrix);LWF=1.0f/LWF;
	LUV2[0]=LXF*LWF;LUV2[1]=LYF*LWF;
	glTexCoord2fv(LUV2);

	glVertex3fv( LVertexNode->GLVertex );
       }
      }

     glEnd();

    }
    else
    {
// No back-face shadow elimination, so we can use OpenGL's
// TEXTURE matrix to do the shadow-map projection!
//
     glPushAttrib(GL_TRANSFORM_BIT);
     glMatrixMode(GL_TEXTURE);
     glPushMatrix();
     glLoadMatrixf(LLocalToShadowMapSTMatrix);

// Start drawing
//
     glBegin(GL_TRIANGLES);

      for(LPCnt=0; LPCnt < LPNum; LPCnt++, LPolygon++)
      {
       LVertexNode=LPolygon->VertexNodes;

       glNormal3fv( LVertexNode->GLNormal );
       glTexCoord3fv( LVertexNode->GLVertex );
       glVertex3fv( LVertexNode->GLVertex );
       LVertexNode++;


       glNormal3fv( LVertexNode->GLNormal );
       glTexCoord3fv( LVertexNode->GLVertex );
       glVertex3fv( LVertexNode->GLVertex );
       LVertexNode++;

       glNormal3fv( LVertexNode->GLNormal );
       glTexCoord3fv( LVertexNode->GLVertex );
       glVertex3fv( LVertexNode->GLVertex );
      }

     glEnd();

     glPopMatrix();

     glMatrixMode(GL_MODELVIEW);
    }
   }
   else
   {
// Normal rendering pass
//
    glBegin(GL_TRIANGLES);

    if(LTextureOn)
    {
     for(LPCnt=0;LPCnt<LPNum;LPCnt++,LPolygon++)
     {
      LVertexNode=LPolygon->VertexNodes;
      glNormal3fv( LVertexNode->GLNormal );

      glTexCoord2fv(LVertexNode->ST);
      glVertex3fv( LVertexNode->GLVertex );LVertexNode++;

      glNormal3fv( LVertexNode->GLNormal );
      glTexCoord2fv(LVertexNode->ST);
      glVertex3fv( LVertexNode->GLVertex );LVertexNode++;

      glNormal3fv( LVertexNode->GLNormal );
      glTexCoord2fv(LVertexNode->ST);
      glVertex3fv( LVertexNode->GLVertex );
     }
    }
    else
    {
     for(LPCnt=0;LPCnt<LPNum;LPCnt++,LPolygon++)
     {
      LVertexNode=LPolygon->VertexNodes;
      glNormal3fv( LVertexNode->GLNormal );
      glVertex3fv( LVertexNode->GLVertex );LVertexNode++;
      glNormal3fv( LVertexNode->GLNormal );
      glVertex3fv( LVertexNode->GLVertex );LVertexNode++;
      glNormal3fv( LVertexNode->GLNormal );
      glVertex3fv( LVertexNode->GLVertex );
     }
    }

    glEnd();

   }
  }

 }
}
