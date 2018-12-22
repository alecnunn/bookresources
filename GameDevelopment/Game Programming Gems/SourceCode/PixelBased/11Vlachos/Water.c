/* Copyright (C) Alex Vlachos, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Alex Vlachos, 2000"
 */
// INCLUDES ===================================================================
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "ext.h"

// DEFINES ====================================================================
#define WATER_WIDTH 60
#define WATER_HEIGHT 60
#define WATER_DEPTH 0.75f
#define WATER_ASPECT 1.0f
#define ATI_SQRTOFTWOINV 4.94974747f
#ifndef _WIN32
#define TRUE 1
#define FALSE 0
#endif

// STRUCTURES =================================================================
#ifdef _WIN32
#pragma pack (push)
#pragma pack (1) //Don't pad the following structures
#endif

typedef struct
{
   int width;   
   int height;   
   int bitDepth;   
   unsigned char *pixels;
} MyTarga;

//Targa header info
typedef struct _AtiTGAHeaderInfo
{
   unsigned char  idlen;    //length of optional identification sequence
   unsigned char  cmtype;   //indicates whether a palette is present
   unsigned char  imtype;   //image data type (e.g., uncompressed RGB)
   unsigned short cmorg;    //first palette index, if present
   unsigned short cmcnt;    //number of palette entries, if present
   unsigned char  cmsize;   //number of bits per palette entry
   unsigned short imxorg;   //horiz pixel coordinate of lower left of image
   unsigned short imyorg;   //vert pixel coordinate of lower left of image
   unsigned short imwidth;  //image width in pixels
   unsigned short imheight; //image height in pixels
   unsigned char  imdepth;  //image color depth (bits per pixel)
   unsigned char  imdesc;   //image attribute flags
#ifdef _WIN32
} MyTGAHeaderInfo;
#else
} __attribute__((packed)) MyTGAHeaderInfo;
#endif

typedef struct
{
   float x, y, z;
} Vec3;

#ifdef _WIN32
#pragma pack (pop)
#endif

// GLOBALS ====================================================================
PFNGLMULTITEXCOORD2FEXTPROC glMultiTexCoord2f;
PFNGLACTIVETEXTUREEXTPROC glActiveTexture;
int gMultiTexture = FALSE;
int gMaxMultitextureUnits = 0;

PFNGLLOCKARRAYSEXTPROC glLockArrays;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArrays;
PFNGLCLIENTACTIVETEXTUREEXTPROC glClientActiveTexture;
int gCVA = FALSE;
int gCVAEnabled = TRUE;

int gTexEnvCombine = FALSE;
int gParaboloidExt = FALSE;
int gParaboloidWidth = 128;
int gParaboloidHeight = 128;

int gDrawNormals = FALSE;
int gDrawWireframe = FALSE;
int gFresnel = FALSE;

GLuint gReflectionTextureID = 0;
GLuint gRefractionTextureID = 0;

int gWidth = 640;
int gHeight = 480;

//Water surface geometry
int gNumVertices = WATER_WIDTH * WATER_HEIGHT;
int gNumIndices = (WATER_WIDTH-1) * (WATER_HEIGHT-1) * 6;

Vec3          gVertexArray      [WATER_WIDTH*WATER_HEIGHT];
Vec3          gNormalArray      [WATER_WIDTH*WATER_HEIGHT];
unsigned char gColorArray       [WATER_WIDTH*WATER_HEIGHT*4];
float         gForceArray       [WATER_WIDTH*WATER_HEIGHT];
float         gVelocityArray    [WATER_WIDTH*WATER_HEIGHT];
float         gTexCoordArray    [WATER_WIDTH*WATER_HEIGHT*2];
int           gPolygonIndexArray[(WATER_WIDTH-1) * (WATER_HEIGHT-1) * 6];

//Camera position
Vec3 gCamera;
float gYaw = 90.0f;
float gPitch = 15.0f;

//=============================================================================
#define Clamp01f64(val) ((val)<0.0 ? 0.0 : ((val)>1.0 ? 1.0 : (val)))
void UpdateWaterPositions(void)
{
   float d;
   int x, y;
   float deltaTime;
   //float deltaTime2;

   int m = WATER_WIDTH;
   int n = WATER_HEIGHT;
   
   /*****************************/
   /* Initialize forces to 0.0f */
   /*****************************/
   //for (x=0; x<gNumVertices; x++)
   //   gForceArray[x] = 0.0f;
   memset(gForceArray, 0, sizeof(float)*gNumVertices);

   /****************************/
   /* Calculate current forces */
   /****************************/
   for (x=2; x<n-2; x++)
      {
      for (y=2; y<m-2; y++) 
         {
         //   Kernel looks like this:
         //
         //    1/Root2 |    1    | 1/Root2 
         //   ---------+---------+---------
         //       1    |    0    |    1  
         //   ---------+---------+---------
         //    1/Root2 |    1    | 1/Root2 
         //
         d = gVertexArray[m*x + y].z - gVertexArray[m*x + y-1].z;
         gForceArray[m*x + y] -= d;
         gForceArray[m*x + y-1] += d;

         d = gVertexArray[m*x + y].z - gVertexArray[m*(x-1) + y].z;
         gForceArray[m*x + y] -= d;
         gForceArray[m*(x-1) + y] += d;

         d = (gVertexArray[m*x + y].z - gVertexArray[m*x + y+1].z);
         gForceArray[m*x + y] -= d;
         gForceArray[m*x + y+1] += d;

         d = (gVertexArray[m*x + y].z - gVertexArray[m*(x+1) + y].z);
         gForceArray[m*x + y] -= d;
         gForceArray[m*(x+1) + y] += d;

         d = (gVertexArray[m*x + y].z - gVertexArray[m*(x+1) + y+1].z)*ATI_SQRTOFTWOINV;
         gForceArray[m*x + y] -= d;
         gForceArray[m*(x+1) + y+1] += d;

         d = (gVertexArray[m*x + y].z - gVertexArray[m*(x-1) + y-1].z)*ATI_SQRTOFTWOINV;
         gForceArray[m*x + y] -= d;
         gForceArray[m*(x-1) + y-1] += d;

         d = (gVertexArray[m*x + y].z - gVertexArray[m*(x+1) + y-1].z)*ATI_SQRTOFTWOINV;
         gForceArray[m*x + y] -= d;
         gForceArray[m*(x+1) + y-1] += d;

         d = (gVertexArray[m*x + y].z - gVertexArray[m*(x-1) + y+1].z)*ATI_SQRTOFTWOINV;
         gForceArray[m*x + y] -= d;
         gForceArray[m*(x-1) + y+1] += d;
         }
      }

   /**********************/
   /* Calculate Velocity */
   /**********************/
   deltaTime = 0.045f;
   //deltaTime2 = 1.0f - 0.05f*(float)Clamp01f64(gCurrentTimeInSeconds64f-gLastTimeInSeconds64f); //Damping factor
   for (x=0; x<gNumVertices; x++)
      {
      gVelocityArray[x] += (gForceArray[x] * deltaTime);
      //gVelocityArray[x] *= deltaTime2;
      }

   /***************************/
   /* Calculate new positions */
   /***************************/
   for (x=0; x<gNumVertices; x++)
      gVertexArray[x].z += gVelocityArray[x];
}
#undef Clamp01f

//=============================================================================
void CalculateWaterNormals(void)
{
   int i;
   int j;
   float tmpf;

   int m = WATER_WIDTH;
   int n = WATER_HEIGHT;
   
   Vec3 *vert;
   Vec3 *normal;

   vert = gVertexArray;
   normal = gNormalArray;
   for (i=0; i<n; i++)
      {
      for (j=0; j<m; j++)
         {
         normal->x = 0.0f;
         normal->y = 0.0f;
         normal->z = 1.0f;

         //Above
         if (i != 0)
            {
            if (j != 0)
               {
               normal->x += -vert[-m-1].z;
               normal->y += -vert[-m-1].z;
               }
            else
               {
               normal->x += -vert[-m].z;
               normal->y += -vert[-m].z;
               }

            normal->x += -vert[-m].z*2.0f;

            if (j != m-1)
               {
               normal->x += -vert[-m+1].z;
               normal->y += vert[-m+1].z;
               }
            else
               {
               normal->x += -vert[-m].z;
               normal->y += vert[-m].z;
               }
            }
         else
            {
            normal->x += -vert[0].z;
            normal->x += -vert[0].z*2.0f;
            normal->x += -vert[0].z;

            normal->y += -vert[0].z;
            normal->y += vert[0].z;
            }

         //Current line
         if (j != 0)
            normal->y += -vert[-1].z*2.0f;
         else
            normal->y += -vert[0].z*2.0f;

         if (j != m-1)
            normal->y += vert[1].z*2.0f;
         else
            normal->y += vert[0].z*2.0f;

         //Below
         if (i != n-1)
            {
            if (j != 0)
               {
               normal->x += vert[m-1].z;
               normal->y += -vert[m-1].z;
               }
            else
               {
               normal->x += vert[m].z;
               normal->y += -vert[m].z;
               }

            normal->x += vert[m].z*2.0f;

            if (j != m-1)
               {
               normal->x += vert[m+1].z;
               normal->y += vert[m+1].z;
               }
            else
               {
               normal->x += vert[m].z;
               normal->y += vert[m].z;
               }
            }
         else
            {
            normal->x += vert[0].z;
            normal->x += vert[0].z*2.0f;
            normal->x += vert[0].z;

            normal->y += -vert[0].z;
            normal->y += vert[0].z;
            }

         //Normalize
         tmpf = 1.0f / (float)sqrt(normal->x*normal->x + normal->y*normal->y + 1.0f);
         normal->x *= tmpf;
         normal->y *= tmpf;
         normal->z *= tmpf;

         vert++;
         normal++;
         }
      }
}

//=============================================================================
void CalculateRefractionCoordinates(void)
{
   int i;
   int j;
   float mInc;
   float nInc;

   int m = WATER_WIDTH;
   int n = WATER_HEIGHT;
   
   Vec3 *vert;
   Vec3 *normal;
   unsigned char *color;
   float *texCoord;

   vert = gVertexArray;
   normal = gNormalArray;
   color = gColorArray;
   texCoord = gTexCoordArray;

   mInc = 1.0f/(m-1.0f);
   nInc = 1.0f/(n-1.0f);
   for (i=0; i<n; i++)
      {
      for (j=0; j<m; j++)
         {
         float tmpf;
         float tmpf2;
         Vec3 eyeVec;
         Vec3 norm;

         Vec3 tmpv;
         float dist[5];
         int tmpi;

         /********************************/
         /* Calculate refraction vectors */
         /********************************/
         //Eye vector to this vertex
         eyeVec.x = gCamera.x - vert->x;
         eyeVec.y = gCamera.y - vert->y;
         eyeVec.z = gCamera.z - vert->z;
         tmpf = 1.0f / (float)sqrt(eyeVec.x*eyeVec.x + eyeVec.y*eyeVec.y + eyeVec.z*eyeVec.z);
         eyeVec.x *= tmpf;
         eyeVec.y *= tmpf;
         eyeVec.z *= tmpf;

         //N.I
         tmpf = normal->x*eyeVec.x + normal->y*eyeVec.y + normal->z*eyeVec.z;
         tmpf2 = 1.0f - ((0.75f*0.75f) * (1.0f-(tmpf*tmpf)));
         tmpf2 = (float)sqrt(tmpf2);

         tmpf2 = (0.75f*tmpf) - tmpf2;
         norm.x = (tmpf2*normal->x) - (0.75f*eyeVec.x);
         norm.y = (tmpf2*normal->y) - (0.75f*eyeVec.y);
         norm.z = (tmpf2*normal->z) - (0.75f*eyeVec.z);
         tmpf2 = 1.0f / (float)sqrt(norm.x*norm.x + norm.y*norm.y + norm.z*norm.z);
         norm.x *= tmpf2;
         norm.y *= tmpf2;
         norm.z *= tmpf2;

         /***************************/
         /* Calculate Fresnel terms */
         /***************************/
         if (gFresnel == TRUE)
            color[0] = (unsigned char)((1.0f - (tmpf*0.8f + 0.1f)) * 255.0f);
         else
            color[0] = 200;
         color[1] = color[0];
         color[2] = color[0];
         color[3] = color[0];

         /*****************************/
         /* Calculate Refraction UV's */
         /*****************************/
         //Point on surface
         tmpv.x = ((j*mInc)*0.998f+0.001f) * WATER_ASPECT;
         tmpv.y = (i*nInc)*0.998f+0.001f;
         tmpv.z = 0.0f;

         /***************************************/
         /* Intersect with left plane (-1 0 0 0) */
         /***************************************/
         if (norm.x == 0.0f) //Parallel to plane
            dist[0] = 999999999.9f;
         else
            dist[0] = (tmpv.x) / (-norm.x);
         if (dist[0] < 0.0f) //If behind ray
            dist[0] = 999999999.9f;

         /****************************************/
         /* Intersect with right plane (-1 0 0 -1) */
         /****************************************/
         if (norm.x == 0.0f) //Parallel to plane
            dist[1] = 999999999.9f;
         else
            dist[1] = (-WATER_ASPECT + tmpv.x) / (-norm.x);
         if (dist[1] < 0.0f) //If behind ray
            dist[1] = 999999999.9f;

         /*****************************************/
         /* Intersect with bottom plane (0 -1 0 0) */
         /*****************************************/
         if (norm.y == 0.0f) //Parallel to plane
            dist[2] = 999999999.9f;
         else
            dist[2] = (tmpv.y) / (-norm.y);
         if (dist[2] < 0.0f) //If behind ray
            dist[2] = 999999999.9f;

         /**************************************/
         /* Intersect with top plane (0 -1 0 -1) */
         /**************************************/
         if (norm.y == 0.0f) //Parallel to plane
            dist[3] = 999999999.9f;
         else
            dist[3] = (-1.0f + tmpv.y) / (-norm.y);
         if (dist[3] < 0.0f) //If behind ray
            dist[3] = 999999999.9f;

         /****************************************/
         /* Intersect with floor plane (0 0 -1 1) */
         /****************************************/
         dist[4] = (WATER_DEPTH + tmpv.z) / (-norm.z);
         if (dist[4] < 0.0f) //If behind ray
            dist[4] = 999999999.9f;

         /*********************/
         /* Find closest wall */
         /*********************/
         tmpi = 0;
         if (dist[1] < dist[tmpi])
            tmpi = 1;
         if (dist[2] < dist[tmpi])
            tmpi = 2;
         if (dist[3] < dist[tmpi])
            tmpi = 3;
         if (dist[4] < dist[tmpi])
            tmpi = 4;

         /*********/
         /* Floor */
         /*********/
         if (tmpi == 4)
            {
            tmpv.x += norm.x*dist[tmpi];
            tmpv.y += norm.y*dist[tmpi];
            tmpv.z += norm.z*dist[tmpi];

            texCoord[0] = tmpv.x/WATER_ASPECT*0.5f + 0.25f;
            texCoord[1] = tmpv.y*0.5f + 0.25f;
            }
         /*************/
         /* Left Wall */
         /*************/
         else if (tmpi == 0)
            {
            tmpv.x += norm.x*dist[tmpi];
            tmpv.y += norm.y*dist[tmpi];
            tmpv.z = norm.z*dist[tmpi];

            //Use y and z to figure out texture coords
            texCoord[0] = -0.25f*tmpv.z/WATER_DEPTH;
            texCoord[1] = ((texCoord[0])) + (tmpv.y*((1.0f-2.0f*texCoord[0])));
            }
         /**************/
         /* Right Wall */
         /**************/
         else if (tmpi == 1)
            {
            tmpv.x += norm.x*dist[tmpi];
            tmpv.y += norm.y*dist[tmpi];
            tmpv.z = norm.z*dist[tmpi];

            //Use y and z to figure out texture coords
            texCoord[0] = -0.25f*tmpv.z/WATER_DEPTH;
            texCoord[1] = ((texCoord[0])) + (tmpv.y*((1.0f-2.0f*texCoord[0])));
            texCoord[0] = 1.0f - texCoord[0];
            }
         /***************/
         /* Bottom Wall */
         /***************/
         else if (tmpi == 2)
            {
            tmpv.x += norm.x*dist[tmpi];
            tmpv.y += norm.y*dist[tmpi];
            tmpv.z = norm.z*dist[tmpi];

            //Use x and z to figure out texture coords
            texCoord[1] = -0.25f*tmpv.z/WATER_DEPTH;
            texCoord[0] = ((texCoord[1])) + (tmpv.x/WATER_ASPECT*((1.0f-2.0f*texCoord[1])));
            }
         /************/
         /* Top Wall */
         /************/
         else if (tmpi == 3)
            {
            tmpv.x += norm.x*dist[tmpi];
            tmpv.y += norm.y*dist[tmpi];
            tmpv.z = norm.z*dist[tmpi];

            //Use x and z to figure out texture coords
            texCoord[1] = -0.25f*tmpv.z/WATER_DEPTH;
            texCoord[0] = ((texCoord[1])) + (tmpv.x/WATER_ASPECT*((1.0f-2.0f*texCoord[1])));
            texCoord[1] = 1.0f - texCoord[1];
            }

         vert++;
         normal++;
         color += 4;
         texCoord += 2;
         }
      }
}

//========================================================================
void Swapf(float *a, float *b)
{
   float temp;

   temp = *a;
   *a = *b;
   *b = temp;
}

//========================================================================
void InvertAffine(float mat[16])
{
   mat[3] = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0.0f;

   Swapf(&(mat[1]), &(mat[4]));
   Swapf(&(mat[2]), &(mat[8]));
   Swapf(&(mat[6]), &(mat[9]));
}

//========================================================================
void SetTextureMatrix(float paraboloidOrientation)
{
   float po = paraboloidOrientation;

   float M[16];
   float P[16]  = {   1, 0, 0, 0,   0,   1, 0, 0,   0, 0, 1, 1,     0,   0, 0, 0  };
   float S[16]  = {  -1, 0, 0, 0,   0,  -1, 0, 0,   0, 0, 1, 0,     0,   0,po, 1  };
   float T[16]  = { 0.5, 0, 0, 0,   0, 0.5, 0, 0,   0, 0, 1, 0,   0.5, 0.5, 0, 1  };
   float C[16]  = {  po, 0, 0, 0,   0,  po, 0, 0,   0, 0, 1, 0,     0,   0, 0, po };
   float C2[16] = {   1, 0, 0, 0,   0,  0, -1, 0,   0, 1, 0, 0,     0,   0, 0, 1  };

   //Grab the modelview matrix
   glGetFloatv(GL_MODELVIEW_MATRIX, M);
   InvertAffine(M);

   //Change to texture matrix
   glMatrixMode(GL_TEXTURE);

   glLoadIdentity();
   glMultMatrixf(C);
   glMultMatrixf(T);
   glMultMatrixf(P);
   glMultMatrixf(S);
   glMultMatrixf(M);

   glMatrixMode(GL_MODELVIEW);
}

//=============================================================================
void DrawWaterSurface(void)
{
   int i;
   int tmpi;

   /*********************/
   /* Single Pass Water */
   /*********************/
   if ((gMultiTexture == TRUE) && (gTexEnvCombine == TRUE))
      {
      //Enable first multitexture stage
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

      glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
      glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
      glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_ALPHA);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
      glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.0f);

      glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PRIMARY_COLOR_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
      glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1.0f);

      SetTextureMatrix(1.0f);
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      glEnable(GL_TEXTURE_GEN_R);

      glBindTexture(GL_TEXTURE_2D, gReflectionTextureID);

      //Enable second multitexture stage
      glActiveTexture(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

      glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
      glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
      glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);
      glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.0f);

      glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PREVIOUS_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
      glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1.0f);

      glBindTexture(GL_TEXTURE_2D, gRefractionTextureID);
      glActiveTexture(GL_TEXTURE0_ARB);

      //Set up arrays
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(Vec3), &gVertexArray);

      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, sizeof(Vec3), &gNormalArray);

      glClientActiveTexture(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, sizeof(float)*2, &gTexCoordArray);

      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(unsigned char)*4, &gColorArray);
      glClientActiveTexture(GL_TEXTURE0_ARB);

      //Lock Arrays
      if ((gCVA == TRUE) && (gCVAEnabled == TRUE))
         glLockArrays(0, gNumVertices);

      //Draw entire surface
      glDrawElements(GL_TRIANGLES, gNumIndices, GL_UNSIGNED_INT, gPolygonIndexArray);

      //Unlock Arrays
      if ((gCVA == TRUE) && (gCVAEnabled == TRUE))
         glUnlockArrays();

      //Disable arrays
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
      glClientActiveTexture(GL_TEXTURE1_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
      glClientActiveTexture(GL_TEXTURE0_ARB);

      glDisable(GL_BLEND);
      
      glActiveTexture(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_GEN_R);
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);

      glActiveTexture(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glActiveTexture(GL_TEXTURE0_ARB);
      }
   else //2 pass water
      {
      /***************************/
      /* Pass #1: Refraction Map */
      /***************************/
      glBindTexture(GL_TEXTURE_2D, gRefractionTextureID);
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(Vec3), &gVertexArray);

      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, sizeof(float)*2, &gTexCoordArray);

      //Lock Arrays
      if ((gCVA == TRUE) && (gCVAEnabled == TRUE))
         glLockArrays(0, gNumVertices);

      //Draw entire surface
      glDrawElements(GL_TRIANGLES, gNumIndices, GL_UNSIGNED_INT, gPolygonIndexArray);

      //Unlock Arrays
      if ((gCVA == TRUE) && (gCVAEnabled == TRUE))
         glUnlockArrays();

      //Disable arrays
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

      glDisable(GL_TEXTURE_2D);

      /***************************/
      /* Pass #2: Reflection Map */
      /***************************/
      glBindTexture(GL_TEXTURE_2D, gReflectionTextureID);
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);

      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(Vec3), &gVertexArray);

      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, sizeof(Vec3), &gNormalArray);

      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(unsigned char)*4, &gColorArray);

      SetTextureMatrix(1.0f);
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      glEnable(GL_TEXTURE_GEN_R);

      //Lock Arrays
      if ((gCVA == TRUE) && (gCVAEnabled == TRUE))
         glLockArrays(0, gNumVertices);

      //Draw entire surface
      glDrawElements(GL_TRIANGLES, gNumIndices, GL_UNSIGNED_INT, gPolygonIndexArray);

      //Unlock Arrays
      if ((gCVA == TRUE) && (gCVAEnabled == TRUE))
         glUnlockArrays();

      //Disable arrays
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);

      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BLEND);

      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_GEN_R);
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      }

   /****************/
   /* Draw Normals */
   /****************/
   if (gDrawNormals == TRUE)
      {
      glBegin(GL_LINES);
      for (i=0; i<gNumVertices; i++)
         {
         Vec3 v;

         v.x = gVertexArray[i].x;
         v.y = gVertexArray[i].y;
         v.z = gVertexArray[i].z;
         glColor3f(0.0f, 0.0f, 1.0f);
         glVertex3f(v.x, v.y, v.z);   

         v.x += (gNormalArray[i].x * 0.1f);
         v.y += (gNormalArray[i].y * 0.1f);
         v.z += (gNormalArray[i].z * 0.1f);
         glColor3f(1.0f, 0.0f, 0.0f);
         glVertex3f(v.x, v.y, v.z);   
         }
      glEnd();
      }

   /******************/
   /* Draw Wireframe */
   /******************/
   if (gDrawWireframe == TRUE)
      {
      glPolygonMode(GL_FRONT, GL_LINE);
      glEnable(GL_POLYGON_OFFSET_LINE);
      glPolygonOffset(-1.0f, 1.0f);

      //Set line width
      glLineWidth(1.0f);

      /**************/
      /* Draw lines */
      /**************/
      glColor3f (1.0f, 0.0f, 0.0f);
      glBegin(GL_TRIANGLES);
      for (i=0; i<gNumIndices; i++)
         {
         tmpi = gPolygonIndexArray[i];
         glVertex3f(gVertexArray[tmpi].x,
                    gVertexArray[tmpi].y,
                    gVertexArray[tmpi].z);
         }
      glEnd();

      glEnable(GL_CULL_FACE);
      glPolygonMode(GL_FRONT, GL_FILL);
      glDisable(GL_POLYGON_OFFSET_LINE);
      }
}

//=============================================================================
// Scene drawing function
//=============================================================================
void Draw(void)
{
   float matrix[16];

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0f, 0.0f, -3.0f);
   glRotatef(gPitch, 1.0f, 0.0f, 0.0f);
   glRotatef(gYaw, 0.0f, 1.0f, 0.0f);
   glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

   //Get camera position
   glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
   gCamera.x = 3.0f * matrix[2];
   gCamera.y = 3.0f * matrix[6];
   gCamera.z = 3.0f * matrix[10];

   //Update and draw water
   UpdateWaterPositions();
   CalculateWaterNormals();
   CalculateRefractionCoordinates();
   DrawWaterSurface();

   //Swap buffers
   glutSwapBuffers();
}

//=============================================================================
// Read a 24 or 32 bit Targa image into a GL-friendly structure
//=============================================================================
int ReadTarga(MyTarga *tga, char *fileName)
{
   int i;
   int numTexels;
   int numBytes;
   int bytesRead;
   unsigned char *tmpPtr, *tmpPtr2, tmp;
   FILE *fp;
   MyTGAHeaderInfo TGAHeader;

   tga->pixels = NULL;

   /*************************************************/
   /* Open texture map file for binary data reading */
   /*************************************************/
   if ((fp = fopen (fileName, "rb")) == NULL)
      {
      printf("ERROR! Could not open texture map \"%s\"\n", fileName);
      return FALSE;
      }

   /*********************/
   /* Read TARGA header */
   /*********************/
   if ((bytesRead = fread (&TGAHeader, sizeof (unsigned char), sizeof (TGAHeader), fp)) != sizeof (TGAHeader))
      {
      printf("ERROR! Bad Targa header!\n");
      return FALSE;
      }

   tga->width = TGAHeader.imwidth;
   tga->height = TGAHeader.imheight;
   tga->bitDepth = TGAHeader.imdepth;
   numTexels = tga->width * tga->height * (tga->bitDepth/8);

   /***********************************************************************/
   /* Skip descriptive bytes at end of header, idlen specifies the number */
   /***********************************************************************/
   if (fseek (fp, TGAHeader.idlen, SEEK_CUR) != 0)
      {
      printf("ERROR! Couldn't seek past Targa header!\n");
      return FALSE;
      }

   /******************************/
   /* Allocate memory for texels */
   /******************************/
   tga->pixels = NULL;
   tga->pixels = (unsigned char *) malloc (sizeof(unsigned char)*numTexels);
   if (tga->pixels == NULL)
      {
      printf("ERROR allocating memory for pixels from file \"%s\"\n", fileName);
      return FALSE;
      }

   /***************************/
   /* Read texels into memory */
   /***************************/
   if ((bytesRead = fread (tga->pixels, sizeof(unsigned char), numTexels, fp)) != numTexels)
      {      
      printf("ERROR! Couldn't read texels!\n");
      return FALSE;
      }

   /**************/
   /* Close file */
   /**************/
   fclose(fp);

   /*********************/
   /* Change BGR to RGB */
   /*********************/
   if (tga->bitDepth == 24)
      {
      tmpPtr = &(tga->pixels[0]);
      tmpPtr2 = &(tga->pixels[2]);

      numBytes = tga->width*tga->height;
      for (i=0; i<numBytes; i++)
         {
         tmp = *tmpPtr;
         *tmpPtr = *tmpPtr2;
         *tmpPtr2 = tmp;

         tmpPtr += 3;
         tmpPtr2 += 3;
         }
      }

   /***********************/
   /* Change BGRA to RGBA */
   /***********************/
   if (tga->bitDepth == 32)
      {
      tmpPtr = &(tga->pixels[0]);
      tmpPtr2 = &(tga->pixels[2]);

      numBytes = tga->width*tga->height;
      for (i=0; i<numBytes; i++)
         {
         tmp = *tmpPtr;
         *tmpPtr = *tmpPtr2;
         *tmpPtr2 = tmp;

         tmpPtr += 4;
         tmpPtr2 += 4;
         }
      }

   return TRUE;
}

//===========================================================================
void InitWaterSurfaces(void)
{
   int j;
   int k;
   int x, y;
   int *indexPtr;
   Vec3 dx, dy;
   Vec3 *vertPtr;

   int m = WATER_WIDTH;
   int n = WATER_HEIGHT;

   /**********************/
   /* Calculate vertices */
   /**********************/
   vertPtr = gVertexArray;
   dx.x = ( 1.0f - -1.0f) / (m-1);
   dx.y = (-1.0f - -1.0f) / (m-1);
   dx.z = 0.0f;
   dy.x = (-1.0f - -1.0f) / (n-1);
   dy.y = ( 1.0f - -1.0f) / (n-1);
   dy.z = 0.0f;
   for (j=0; j<n; j++)
      {
      for (k=0; k<m; k++)
         {
         vertPtr->x = -1.0f + dx.x*(float)k + dy.x*(float)j;
         vertPtr->y = -1.0f + dx.y*(float)k + dy.y*(float)j;
         vertPtr->z = 0.0f + dx.z*(float)k + dy.z*(float)j;
         vertPtr++;
         }
      }

   /**********************/
   /* Calculate polygons */
   /**********************/
   x = 0;
   y = m;
   indexPtr = gPolygonIndexArray;
   for (j=0; j<n-1; j++)
      {
      for (k=0; k<m-1; k++)
         {
         indexPtr[0] = x;
         indexPtr[1] = x+1;
         indexPtr[2] = y;
         indexPtr[3] = y;
         indexPtr[4] = x+1;
         indexPtr[5] = y+1;

         indexPtr += 6;
         x++;
         y++;
         }
      x++;
      y++;
      }

   /***********************/
   /* Start water ripples */
   /***********************/
   gVertexArray[m+1].z = 0.5f;
   gVertexArray[m*(n-1)-2].z = 0.5f;
}

//=============================================================================
// GL Initialization
//=============================================================================
int MyInit()
{
   MyTarga tga;

   //Set up OpenGL states
   glEnable(GL_DEPTH_TEST); //Turn on z-buffer
   glFrontFace(GL_CCW);     //Counter Clock-wise polygons face out
   glEnable(GL_CULL_FACE);  //Cull faces based on glFrontFace
   glCullFace(GL_BACK);
   glDepthFunc(GL_LEQUAL);
   glDisable(GL_LINE_SMOOTH);
   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   glDrawBuffer(GL_BACK);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glDisable(GL_TEXTURE_2D);
   glPolygonMode(GL_FRONT, GL_FILL);
   glReadBuffer(GL_BACK);

   /******************************/
   /* Read in reflection texture */
   /******************************/
   if (ReadTarga(&tga, "Reflect.tga") == FALSE)
      {
      printf("Error reading in image \"Reflect.tga\"! Exiting.\n");
      return FALSE;
      }
   glGenTextures(1, &gReflectionTextureID);
   glBindTexture(GL_TEXTURE_2D, gReflectionTextureID);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, tga.width, tga.height, 0, 
      (tga.bitDepth==24)?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, tga.pixels);

   if (tga.pixels != NULL)
      {
      free (tga.pixels);
      tga.pixels = NULL;
      }

   /******************************/
   /* Read in refraction texture */
   /******************************/
   if (ReadTarga(&tga, "Refract.tga") == FALSE)
      {
      printf("Error reading in image \"Refract.tga\"! Exiting.\n");
      return FALSE;
      }
   glGenTextures(1, &gRefractionTextureID);
   glBindTexture(GL_TEXTURE_2D, gRefractionTextureID);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, tga.width, tga.height, 0, 
      (tga.bitDepth==24)?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, tga.pixels);

   if (tga.pixels != NULL)
      {
      free (tga.pixels);
      tga.pixels = NULL;
      }

   //Camera
   gCamera.x = 0.0f;
   gCamera.y = 0.0f;
   gCamera.z = 0.0f;

   InitWaterSurfaces();

   return TRUE;
}

//=============================================================================
void MouseFunc(int button, int state, int x, int y)
{
   static buttonDown = FALSE;
   static lastX = 0;
   static lastY = 0;

   switch(button)
      {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN)
            {
            lastX = x;
            lastY = y;
            buttonDown = TRUE;
            }
         else
            buttonDown = FALSE;
         break;
      }

   if (buttonDown == TRUE)
      {
      gYaw -= (float)(lastX - x) / (float)gWidth * 70.0f;
      gPitch -= (float)(lastY - y) / (float)gHeight * 70.0f;

      if (gYaw >= 360.0f)
         gYaw -= 360.0f;
      if (gYaw <= 0.0f)
         gYaw += 360.0f;

      if (gPitch < 0.0f)
         gPitch = 0.0f;
      if (gPitch > 90.0f)
         gPitch = 90.0f;

      lastX = x;
      lastY = y;
      }
}

//=============================================================================
void MotionFunc(int x, int y)
{
   MouseFunc(999, 0, x, y);
}

//=============================================================================
// Key input handler
//=============================================================================
void KeyFunc(unsigned char key, int x, int y)
{
   switch(key)
      {
      //Quit
      case '\x1b':
      case 'Q':
      case 'q':
         exit(0);
         break;

      case 'n':
      case 'N':
         gDrawNormals ^= TRUE;
         break;

      case 'w':
      case 'W':
         gDrawWireframe ^= TRUE;
         break;

      case 'f':
      case 'F':
         gFresnel ^= TRUE;
         if (gFresnel == TRUE)
            printf("Fresnel term ON\n");
         else
            printf("Fresnel term OFF\n");
         break;

      case '4':
         gYaw += 1.0f;
         if (gYaw >= 360.0f)
            gYaw -= 360.0f;
         if (gYaw <= 0.0f)
            gYaw += 360.0f;
         break;

      case '6':
         gYaw -= 1.0f;
         if (gYaw >= 360.0f)
            gYaw -= 360.0f;
         if (gYaw <= 0.0f)
            gYaw += 360.0f;
         break;

      case '8':
         gPitch += 1.0f;
         if (gPitch < 0.0f)
            gPitch = 0.0f;
         if (gPitch > 90.0f)
            gPitch = 90.0f;
         break;

      case '2':
         gPitch -= 1.0f;
         if (gPitch < 0.0f)
            gPitch = 0.0f;
         if (gPitch > 90.0f)
            gPitch = 90.0f;
         break;

      case 'v':
      case 'V':
         gCVAEnabled ^= TRUE;
         if (gCVAEnabled == TRUE)
            printf("Compiled Vertex Arrays ON\n");
         else
            printf("Compiled Vertex Arrays OFF\n");
         break;
      }
}

//=============================================================================
int ExtensionSupported(const char *extension)
{
   static const char *extensions = NULL;
   const char *start;
   char *where, *terminator;
   
   where = (char *) strchr(extension, ' ');
   if ((where) || (*extension == '\0'))
      return FALSE;
   
   if (!extensions)
      extensions = (const char *) glGetString(GL_EXTENSIONS);
   
   start = extensions;
   for (;;) 
      {
      where = (char *) strstr((const char *) start, extension);
      if (!where)
        break;

      terminator = where + strlen(extension);
      if (where == start || *(where - 1) == ' ') 
         {
         if (*terminator == ' ' || *terminator == '\0') 
            {
            return TRUE;
            }
         }

      start = terminator;
      }

   return FALSE;
}

//=============================================================================
void InitExtensions(void)
{
#ifndef _WIN32
   void *libHandle;
#endif

   if (ExtensionSupported("GL_ARB_multitexture") == TRUE)
      {
      gMultiTexture = TRUE;
#ifdef _WIN32
      glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FEXTPROC) wglGetProcAddress("glMultiTexCoord2fARB");
      glActiveTexture = (PFNGLACTIVETEXTUREEXTPROC) wglGetProcAddress("glActiveTextureARB");
      glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREEXTPROC) wglGetProcAddress("glClientActiveTextureARB");
#else
      libHandle = dlopen(NULL, RTLD_LAZY);
      glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FEXTPROC) dlsym(libHandle, "glMultiTexCoord2fARB");
      glActiveTexture = (PFNGLACTIVETEXTUREEXTPROC) dlsym(libHandle, "glActiveTextureARB");
      glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREEXTPROC) dlsym(libHandle, "glClientActiveTextureARB");
      dlclose(libHandle);
#endif

      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &gMaxMultitextureUnits);
      }

   if (ExtensionSupported("GL_EXT_texture_env_combine") == TRUE)
      {
      gTexEnvCombine = TRUE;
      }

   if (ExtensionSupported("GL_EXT_compiled_vertex_array") == TRUE)
      {
      gCVA = TRUE;
#ifdef _WIN32
      glLockArrays = (PFNGLLOCKARRAYSEXTPROC) wglGetProcAddress("glLockArraysEXT");
      glUnlockArrays = (PFNGLUNLOCKARRAYSEXTPROC) wglGetProcAddress("glUnlockArraysEXT");
#else
      libHandle = dlopen(NULL, RTLD_LAZY);
      glLockArrays = (PFNGLLOCKARRAYSEXTPROC) dlsym(libHandle, "glLockArraysEXT");
      glUnlockArrays = (PFNGLUNLOCKARRAYSEXTPROC) dlsym(libHandle, "glUnlockArraysEXT");
      dlclose(libHandle);
#endif
      }

   if ((ExtensionSupported("GL_EXT_texgen_reflection") == TRUE) || (ExtensionSupported("GL_NV_texgen_reflection") == TRUE))
      {
      gParaboloidExt = TRUE;

      if (gMultiTexture == TRUE)
         {
         glActiveTexture(GL_TEXTURE0_ARB);
         glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
         glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
         glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);

         glActiveTexture(GL_TEXTURE1_ARB);
         glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
         glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
         glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);

         glActiveTexture(GL_TEXTURE0_ARB);
         }
      else
         {
         glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
         glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
         glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
         }
      }
}

//=============================================================================
// Projection and View Setup
//=============================================================================
void MyReshape(int w, int h)
{
   gWidth = w;
   gHeight = h;

   glViewport(0,0,w,h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(40.0, (double)w/(double)h, 0.01, 100.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0f,0.0f,-5.0f);
}

//=============================================================================
int main(int argc, char **argv)
{
   printf("Water Refraction Gem\n");
   printf("April 28, 2000\n");
   printf("Authors: Alex Vlachos, Jason Mitchell\n");
   printf("\n");
   printf(" ESC      - Quit program\n");
   printf(" NumPad 4 - Rotate left\n");
   printf(" NumPad 6 - Rotate right\n");
   printf(" NumPad 8 - Rotate up\n");
   printf(" NumPad 2 - Rotate down\n");
   printf(" Left Mouse Button & Drag - Rotate\n");
   printf("\n");
   printf(" F - Toggle Fresnel Term (Default Off)\n");
   printf(" N - Toggle Normals (Default Off)\n");
   printf(" W - Toggle Wireframe (Default Off)\n");
   printf(" V - Toggle Compiled Vertex Arrays (Default On)\n");

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   glutCreateWindow("Water Refraction Gem");
   glutReshapeWindow(gWidth, gHeight);
   InitExtensions();
   if (gParaboloidExt == FALSE)
      {
      printf("\nERROR! Cannot find GL_EXT_texgen_reflection or GL_NV_texgen_reflection extension\n");
      return;
      }
   glutDisplayFunc(Draw);
   glutReshapeFunc(MyReshape);
   glutKeyboardFunc(KeyFunc);
   glutMouseFunc(MouseFunc);
   glutMotionFunc(MotionFunc);
   glutIdleFunc(Draw);

   if (MyInit() == FALSE)
      return;

   glutMainLoop();
}
