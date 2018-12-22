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
/* DATE:	2000-May-02 14:44:06						*/
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
#include "str.h"

/*======================================*/
/* Initialize light			*/
/*======================================*/
void E3d_LightDefault(E3dLight* LLight)
{
 LLight->Name=NULL;
 LLight->Type=E3dLIGHT_POINT;
 LLight->Position.X=0.0;
 LLight->Position.Y=0.0;
 LLight->Position.Z=0.0;
 LLight->Color.R=1.0;
 LLight->Color.G=1.0;
 LLight->Color.B=1.0;
 LLight->ConstAttenuation=1.0;
 LLight->LinAttenuation=0.0;
 LLight->SquareAttenuation=0.0;
}


/*======================================*/
/* Update a GL light			*/
/*======================================*/
void E3d_GLUpdateLight(int LGLLight, E3dLight* LLight)
{
 GLfloat	LPosition[4];
 GLfloat	LColor[4];

 LPosition[0]=LLight->Position.X;
 LPosition[1]=LLight->Position.Y;
 LPosition[2]=LLight->Position.Z;
 LPosition[3]=1.0;
 LColor[0]=LLight->Color.R;
 LColor[1]=LLight->Color.G;
 LColor[2]=LLight->Color.B;
 LColor[3]=1.0;
 glLightfv (GL_LIGHT0+LGLLight, GL_POSITION, LPosition);
 glLightfv (GL_LIGHT0+LGLLight, GL_DIFFUSE, LColor);
 glLightfv (GL_LIGHT0+LGLLight, GL_SPECULAR, LColor);
}


/*======================================*/
/* Initialize a material to default	*/
/*======================================*/
void E3d_MaterialDefault(E3dMaterial* LMaterial)
{
 LMaterial->Name=NULL;
 LMaterial->Type=E3dMAT_PHONG;
 LMaterial->Ambient[0]=0.2f;LMaterial->Ambient[1]=0.2f;LMaterial->Ambient[2]=0.4f;LMaterial->Ambient[3]=1.0f;
 LMaterial->Diffuse[0]=0.15f;LMaterial->Diffuse[1]=0.7f;LMaterial->Diffuse[2]=0.8f;LMaterial->Diffuse[3]=1.0f;
 LMaterial->Specular[0]=0.3f;LMaterial->Specular[1]=0.9f;LMaterial->Specular[2]=1.0f;LMaterial->Specular[3]=1.0f;
 LMaterial->Emission[0]=0.0f;LMaterial->Emission[1]=0.0f;LMaterial->Emission[2]=0.0f;LMaterial->Emission[3]=1.0f;
 LMaterial->Specularity=10.0;
// LMaterial->NumOf2DTextures=0;LMaterial->Textures2D=NULL;

}


/*======================================*/
/* Allocate a material			*/
/*======================================*/
E3dMaterial* E3d_MaterialAllocate()
{ 
 E3dMaterial*	LMaterial;

 if((LMaterial=(E3dMaterial*)EMalloc(sizeof(E3dMaterial)))!=NULL)
 {
  LMaterial->RefCnt=0;
  E3d_MaterialDefault(LMaterial);

  return(LMaterial);
 }
 return(NULL);
}





/*======================================*/
/* Dump the contents of a Matrix	*/
/*======================================*/
void E3d_PrintMatrix(E3dMatrix LMatrix)
{
 printf("%f,%f,%f,%f\n",LMatrix[M00],LMatrix[M01],LMatrix[M02],LMatrix[M03]);
 printf("%f,%f,%f,%f\n",LMatrix[M10],LMatrix[M11],LMatrix[M12],LMatrix[M13]);
 printf("%f,%f,%f,%f\n",LMatrix[M20],LMatrix[M21],LMatrix[M22],LMatrix[M23]);
 printf("%f,%f,%f,%f\n",LMatrix[M30],LMatrix[M31],LMatrix[M32],LMatrix[M33]);
 fflush(stdout);
}


/*======================================*/
/* Copy a matrix			*/
/*======================================*/
void E3d_MatrixCopy(E3dMatrix LSrcMtx,E3dMatrix LDestMtx)
{
 LDestMtx[M00]=LSrcMtx[M00];LDestMtx[M01]=LSrcMtx[M01];LDestMtx[M02]=LSrcMtx[M02];LDestMtx[M03]=LSrcMtx[M03];
 LDestMtx[M10]=LSrcMtx[M10];LDestMtx[M11]=LSrcMtx[M11];LDestMtx[M12]=LSrcMtx[M12];LDestMtx[M13]=LSrcMtx[M13];
 LDestMtx[M20]=LSrcMtx[M20];LDestMtx[M21]=LSrcMtx[M21];LDestMtx[M22]=LSrcMtx[M22];LDestMtx[M23]=LSrcMtx[M23];
 LDestMtx[M30]=LSrcMtx[M30];LDestMtx[M31]=LSrcMtx[M31];LDestMtx[M32]=LSrcMtx[M32];LDestMtx[M33]=LSrcMtx[M33];
}


/*======================================*/
/* "Translate" a matrix			*/
/*======================================*/
void E3d_MatrixTranslate(E3dMatrix LMatrix,double LX,double LY,double LZ)
{
 E3dCoordType	LDX,LDY,LDZ;

 LDX=(E3dCoordType)(LX*LMatrix[M00]+LY*LMatrix[M10]+LZ*LMatrix[M20]);
 LDY=(E3dCoordType)(LX*LMatrix[M01]+LY*LMatrix[M11]+LZ*LMatrix[M21]);
 LDZ=(E3dCoordType)(LX*LMatrix[M02]+LY*LMatrix[M12]+LZ*LMatrix[M22]);

 LMatrix[M30]+=LDX;
 LMatrix[M31]+=LDY;
 LMatrix[M32]+=LDZ;
}


/*======================================*/
/* Pre-multiple LSrcMtx with LDestMtx	*/
/*======================================*/
void E3d_MatrixMult(E3dMatrix LSrcMtx,E3dMatrix LDestMtx)
{
 E3dCoordType	L00,L01,L02,L03,L10,L11,L12,L13,L20,L21,L22,L23,L30,L31,L32,L33;

 L00=LDestMtx[M00];L01=LDestMtx[M01];L02=LDestMtx[M02];L03=LDestMtx[M03];
 L10=LDestMtx[M10];L11=LDestMtx[M11];L12=LDestMtx[M12];L13=LDestMtx[M13];
 L20=LDestMtx[M20];L21=LDestMtx[M21];L22=LDestMtx[M22];L23=LDestMtx[M23];
 L30=LDestMtx[M30];L31=LDestMtx[M31];L32=LDestMtx[M32];L33=LDestMtx[M33];

 LDestMtx[M00]=LSrcMtx[M00]*L00+LSrcMtx[M01]*L10+LSrcMtx[M02]*L20+LSrcMtx[M03]*L30;
 LDestMtx[M01]=LSrcMtx[M00]*L01+LSrcMtx[M01]*L11+LSrcMtx[M02]*L21+LSrcMtx[M03]*L31;
 LDestMtx[M02]=LSrcMtx[M00]*L02+LSrcMtx[M01]*L12+LSrcMtx[M02]*L22+LSrcMtx[M03]*L32;
 LDestMtx[M03]=LSrcMtx[M00]*L03+LSrcMtx[M01]*L13+LSrcMtx[M02]*L23+LSrcMtx[M03]*L33;

 LDestMtx[M10]=LSrcMtx[M10]*L00+LSrcMtx[M11]*L10+LSrcMtx[M12]*L20+LSrcMtx[M13]*L30;
 LDestMtx[M11]=LSrcMtx[M10]*L01+LSrcMtx[M11]*L11+LSrcMtx[M12]*L21+LSrcMtx[M13]*L31;
 LDestMtx[M12]=LSrcMtx[M10]*L02+LSrcMtx[M11]*L12+LSrcMtx[M12]*L22+LSrcMtx[M13]*L32;
 LDestMtx[M13]=LSrcMtx[M10]*L03+LSrcMtx[M11]*L13+LSrcMtx[M12]*L23+LSrcMtx[M13]*L33;

 LDestMtx[M20]=LSrcMtx[M20]*L00+LSrcMtx[M21]*L10+LSrcMtx[M22]*L20+LSrcMtx[M23]*L30;
 LDestMtx[M21]=LSrcMtx[M20]*L01+LSrcMtx[M21]*L11+LSrcMtx[M22]*L21+LSrcMtx[M23]*L31;
 LDestMtx[M22]=LSrcMtx[M20]*L02+LSrcMtx[M21]*L12+LSrcMtx[M22]*L22+LSrcMtx[M23]*L32;
 LDestMtx[M23]=LSrcMtx[M20]*L03+LSrcMtx[M21]*L13+LSrcMtx[M22]*L23+LSrcMtx[M23]*L33;

 LDestMtx[M30]=LSrcMtx[M30]*L00+LSrcMtx[M31]*L10+LSrcMtx[M32]*L20+LSrcMtx[M33]*L30;
 LDestMtx[M31]=LSrcMtx[M30]*L01+LSrcMtx[M31]*L11+LSrcMtx[M32]*L21+LSrcMtx[M33]*L31;
 LDestMtx[M32]=LSrcMtx[M30]*L02+LSrcMtx[M31]*L12+LSrcMtx[M32]*L22+LSrcMtx[M33]*L32;
 LDestMtx[M33]=LSrcMtx[M30]*L03+LSrcMtx[M31]*L13+LSrcMtx[M32]*L23+LSrcMtx[M33]*L33;
}


/*======================================*/
/* "Rotate" a matrix			*/
/*======================================*/
void E3d_MatrixRotate(E3dMatrix LMatrix,int LAxis,double LAngle)
{
 E3dAngleType	LSinA,LCosA,LMSinA;
 E3dMatrix	LTMtx;

 LSinA=(E3dAngleType)sin(LAngle*E3dRADIANDIV);LMSinA=-LSinA;
 LCosA=(E3dAngleType)cos(LAngle*E3dRADIANDIV);
 switch(LAxis)
 {
  case 'x':
   LTMtx[M10]=LMatrix[M10]*LCosA+LMatrix[M20]*LSinA;LTMtx[M11]=LMatrix[M11]*LCosA+LMatrix[M21]*LSinA;LTMtx[M12]=LMatrix[M12]*LCosA+LMatrix[M22]*LSinA;

   LTMtx[M20]=LMatrix[M10]*LMSinA+LMatrix[M20]*LCosA;LTMtx[M21]=LMatrix[M11]*LMSinA+LMatrix[M21]*LCosA;LTMtx[M22]=LMatrix[M12]*LMSinA+LMatrix[M22]*LCosA;

   LMatrix[M10]=LTMtx[M10];LMatrix[M11]=LTMtx[M11];LMatrix[M12]=LTMtx[M12];
   LMatrix[M20]=LTMtx[M20];LMatrix[M21]=LTMtx[M21];LMatrix[M22]=LTMtx[M22];
  break;

  case 'y':
   LTMtx[M00]=LMatrix[M00]*LCosA+LMatrix[M20]*LMSinA;LTMtx[M01]=LMatrix[M01]*LCosA+LMatrix[M21]*LMSinA;LTMtx[M02]=LMatrix[M02]*LCosA+LMatrix[M22]*LMSinA;
   LTMtx[M20]=LMatrix[M00]*LSinA+LMatrix[M20]*LCosA;LTMtx[M21]=LMatrix[M01]*LSinA+LMatrix[M21]*LCosA;LTMtx[M22]=LMatrix[M02]*LSinA+LMatrix[M22]*LCosA;

   LMatrix[M00]=LTMtx[M00];LMatrix[M01]=LTMtx[M01];LMatrix[M02]=LTMtx[M02];
   LMatrix[M20]=LTMtx[M20];LMatrix[M21]=LTMtx[M21];LMatrix[M22]=LTMtx[M22];
  break;

  case 'z':
   LTMtx[M00]=LMatrix[M00]*LCosA+LMatrix[M10]*LSinA;LTMtx[M01]=LMatrix[M01]*LCosA+LMatrix[M11]*LSinA;LTMtx[M02]=LMatrix[M02]*LCosA+LMatrix[M12]*LSinA;

   LTMtx[M10]=LMatrix[M00]*LMSinA+LMatrix[M10]*LCosA;LTMtx[M11]=LMatrix[M01]*LMSinA+LMatrix[M11]*LCosA;LTMtx[M12]=LMatrix[M02]*LMSinA+LMatrix[M12]*LCosA;

   LMatrix[M00]=LTMtx[M00];LMatrix[M01]=LTMtx[M01];LMatrix[M02]=LTMtx[M02];
   LMatrix[M10]=LTMtx[M10];LMatrix[M11]=LTMtx[M11];LMatrix[M12]=LTMtx[M12];
  break;
 }
}


/*======================================*/
/* Copy identity matrix to a matrix	*/
/*======================================*/
void E3d_MatrixReset(E3dMatrix LDestMtx)
{
 LDestMtx[M00]=1.0;LDestMtx[M01]=0.0;LDestMtx[M02]=0.0;LDestMtx[M03]=0.0;
 LDestMtx[M10]=0.0;LDestMtx[M11]=1.0;LDestMtx[M12]=0.0;LDestMtx[M13]=0.0;
 LDestMtx[M20]=0.0;LDestMtx[M21]=0.0;LDestMtx[M22]=1.0;LDestMtx[M23]=0.0;
 LDestMtx[M30]=0.0;LDestMtx[M31]=0.0;LDestMtx[M32]=0.0;LDestMtx[M33]=1.0;
}






/*======================================*/
/* Reset transformations of a model	*/
/*======================================*/
void E3d_ModelResetTransforms(E3dModel* LModel)
{
 LModel->Translation.X=0.0;
 LModel->Translation.Y=0.0;
 LModel->Translation.Z=0.0;
 LModel->Scaling.X=1.0;
 LModel->Scaling.Y=1.0;
 LModel->Scaling.Z=1.0;
 LModel->Rotation.X=0.0;
 LModel->Rotation.Y=0.0;
 LModel->Rotation.Z=0.0;
 E3d_MatrixReset(LModel->LocalToWorldMatrix);
}


/*======================================*/
/* Allocate a model			*/
/*======================================*/
E3dModel* E3d_ModelAllocate(char* LName)
{
 E3dModel*	LModel;

 if((LModel=(E3dModel*)EMalloc(sizeof(E3dModel)))!=NULL)
 {
  if(LName==NULL) LModel->Name=LName;
  else LModel->Name=strdup(LName);
  LModel->Geometry=NULL;
  LModel->Material=NULL;

#ifdef USEOpenGL
  LModel->GLDisplayListID=0;
  LModel->GLDisplayListUptodate=FALSE;
#endif // USEOpenGL

  E3d_ModelResetTransforms(LModel);

  return(LModel);
 }
 return(NULL);
}


/*======================================*/
/* Free a model				*/
/*======================================*/
void E3d_ModelFree(E3dModel* LModel)
{
 if(LModel->Name!=NULL) EFree(LModel->Name);

// This is not implemented yet...
//
// if(LModel->Geometry!=NULL) E3d_GeometryFree(LModel->Geometry);

// if(LModel->Material!=NULL) E3d_MaterialFree(LModel->Material);

 EFree(LModel);
}


/*==============================================================*/
/* Calculate transformation matrices of a model hierarchy	*/
/*==============================================================*/
void E3d_ModelRefreshMatrices(E3dModel* LModel)
{
 E3dMatrix	LMatrix;

 E3d_MatrixReset(LMatrix);

 E3d_MatrixTranslate(LMatrix,LModel->Translation.X, LModel->Translation.Y, LModel->Translation.Z);

 E3d_MatrixRotate(LMatrix,'z', LModel->Rotation.Z);
 E3d_MatrixRotate(LMatrix,'y', LModel->Rotation.Y);
 E3d_MatrixRotate(LMatrix,'x', LModel->Rotation.X);

// Use matrix without scaling for transforming normals
//
 E3d_MatrixCopy(LMatrix,LModel->NormalLocalToWorldMatrix);

 E3d_MatrixCopy(LMatrix,LModel->LocalToWorldMatrix);
}
