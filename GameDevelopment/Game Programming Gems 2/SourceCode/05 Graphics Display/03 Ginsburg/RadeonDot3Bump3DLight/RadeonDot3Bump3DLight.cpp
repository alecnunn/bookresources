/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
///////////////////////////////////////////////////////////////////////////////
//
//   RadeonDot3Bump.cpp - This sample app shows using DOT3 for bumpmapping.
//                        it packs the light vector into the color and does
//                        a dot with a pre-filtered height map.
//
//   Controls:   [ESC]            - Quit
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h>
#include <stdio.h>
#include "math.h"
#include "glati.h"
#include "wglati.h"
#include "GLTexture.h"
#include "character.h"

#include "BumpObject.h"

// Extension definitions.
#define GL_MIRROR_CLAMP_ATI         0x6080
#define GL_MIRROR_CLAMP_TO_EDGE_ATI 0x6081

// The object to display
BumpObject bumpObject;
char *pofName = NULL;

// The base textures for the object
#define NUM_DEMO_TEXTURES 3
char *textureName[NUM_DEMO_TEXTURES] = {"Fieldstone.tga", "Tile4.tga", 
                                        "White.tga"};
GLuint textureID[NUM_DEMO_TEXTURES];
ATI_GLTexture* texture[NUM_DEMO_TEXTURES];
GLuint currentTexture = 0;

// The pre-filtered heightmap texurue
char *textureDotName[NUM_DEMO_TEXTURES] = {"FieldstoneBumpDOT3.tga", 
                                           "Tile4BumpDOT3.tga",
                                           "WhiteBumpDOT3.tga"};
GLuint textureDotID[NUM_DEMO_TEXTURES];
ATI_GLTexture* textureDot[NUM_DEMO_TEXTURES];

// Cubemap texture
#define CUBEMAP_TEX_SIZE 128
GLuint cubemapID;
GLubyte cubemapPX[3*CUBEMAP_TEX_SIZE*CUBEMAP_TEX_SIZE];
GLubyte cubemapPY[3*CUBEMAP_TEX_SIZE*CUBEMAP_TEX_SIZE];
GLubyte cubemapPZ[3*CUBEMAP_TEX_SIZE*CUBEMAP_TEX_SIZE];
GLubyte cubemapNX[3*CUBEMAP_TEX_SIZE*CUBEMAP_TEX_SIZE];
GLubyte cubemapNY[3*CUBEMAP_TEX_SIZE*CUBEMAP_TEX_SIZE];
GLubyte cubemapNZ[3*CUBEMAP_TEX_SIZE*CUBEMAP_TEX_SIZE];

// 3D Lightmap texture(s?)
#define NUM_LIGHTMAP_TEXTURES 1
GLuint lightmapID[NUM_LIGHTMAP_TEXTURES];
#define SIZE_LIGHTMAP_3D 64
GLubyte lightmap[SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*3];

// Setup text texture
GLuint textID;
ATI_GLTexture* charset;

// Light parameters
GLfloat lightMat[16];
GLfloat lightpos[] = {0.0f, 0.0f, 50.0f, 1.0f};
GLfloat lightDist = 50.0f;

// Viewing parameters
GLfloat viewMat[16];
GLfloat fov = 90.0;
GLfloat clipNear = 0.01;
GLfloat clipFar = 200.0;
GLfloat scaleFactor = 1.0;
GLint width = 640;
GLint height = 480;

// A couple of handy colors
static GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// Flags
GLuint wireframe = GL_FALSE;
GLuint moving = GL_TRUE;
GLuint showVectors = GL_FALSE;
GLuint showTanCoord = GL_FALSE;
GLuint anisotropic = GL_TRUE;
GLuint useCubemap = GL_TRUE;
GLuint texturing = 0; // 0 - Full bump mapping (Dot mod Base).
                      // 1 - Just Light Vector dot Filtered height map.
                      // 2 - Light Vector encoded as color
                      // 3 - Just Filtered height map.
                      // 4 - Just Base map.

// Movement variables
GLint leftX = 0;
GLint leftY = 0;
GLuint leftDown = GL_FALSE;
GLint rightX = 0;
GLint rightY = 0;
GLuint rightDown = GL_FALSE;

// Prototypes needed.
PFNGLACTIVETEXTUREARBPROC glActiveTexture;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture;
PFNGLTEXIMAGE3DEXT glTexImage3DEXT;

//////////////////////////
// Set matrix to identity
//   matrix - Input/Output matrix
//////////////////////////
void 
matrixIdentity(GLfloat matrix[16])
{
  matrix[ 0] = 1.0;  matrix[ 1] = 0.0;  matrix[ 2] = 0.0;  matrix[ 3] = 0.0;
  matrix[ 4] = 0.0;  matrix[ 5] = 1.0;  matrix[ 6] = 0.0;  matrix[ 7] = 0.0;
  matrix[ 8] = 0.0;  matrix[ 9] = 0.0;  matrix[10] = 1.0;  matrix[11] = 0.0;
  matrix[12] = 0.0;  matrix[13] = 0.0;  matrix[14] = 0.0;  matrix[15] = 1.0;
}

//////////////////////////
// Invert a matrix. (Matrix MUST be orhtonormal!)
//   in - Input matrix
//   out - Output matrix
//////////////////////////
void
matrixInvert(GLfloat in[16], GLfloat out[16])
{
  // Transpose rotation
  out[ 0] = in[ 0];  out[ 1] = in[ 4];  out[ 2] = in[ 8];
  out[ 4] = in[ 1];  out[ 5] = in[ 5];  out[ 6] = in[ 9];
  out[ 8] = in[ 2];  out[ 9] = in[ 6];  out[10] = in[10];
  
  // Clear shearing terms
  out[3] = 0.0f; out[7] = 0.0f; out[11] = 0.0f; out[15] = 1.0f;

  // Translation is minus the dot of tranlation and rotations
  out[12] = -(in[12]*in[ 0]) - (in[13]*in[ 1]) - (in[14]*in[ 2]);
  out[13] = -(in[12]*in[ 4]) - (in[13]*in[ 5]) - (in[14]*in[ 6]);
  out[14] = -(in[12]*in[ 8]) - (in[13]*in[ 9]) - (in[14]*in[10]);
}

//////////////////////////
// Multiply a vector by a matrix.
//   vecIn - Input vector
//   m - Input matrix
//   vecOut - Output vector
//////////////////////////
void
vecMatMult(GLfloat vecIn[3], GLfloat m[16], GLfloat vecOut[3]) {
  vecOut[0] = (vecIn[0]*m[ 0]) + (vecIn[1]*m[ 4]) + (vecIn[2]*m[ 8]) + m[12];
  vecOut[1] = (vecIn[0]*m[ 1]) + (vecIn[1]*m[ 5]) + (vecIn[2]*m[ 9]) + m[13];
  vecOut[2] = (vecIn[0]*m[ 2]) + (vecIn[1]*m[ 6]) + (vecIn[2]*m[10]) + m[14];
}

//////////////////////////
// Multiply a vector by just the 3x3 portion of a matrix.
//   vecIn - Input vector
//   m - Input matrix
//   vecOut - Output vector
//////////////////////////
void
vecMat3x3Mult(GLfloat vecIn[3], GLfloat m[16], GLfloat vecOut[3]) {
  vecOut[0] = (vecIn[0]*m[ 0]) + (vecIn[1]*m[ 4]) + (vecIn[2]*m[ 8]);
  vecOut[1] = (vecIn[0]*m[ 1]) + (vecIn[1]*m[ 5]) + (vecIn[2]*m[ 9]);
  vecOut[2] = (vecIn[0]*m[ 2]) + (vecIn[1]*m[ 6]) + (vecIn[2]*m[10]);
}

//////////////////////////
// Setup texture stages for Dot3 Bump-mapping.
//////////////////////////
void
Dot3Bump() {
  // Setup first texture stage to DOT the color (which contains the
  // light vector in tangent space) with the pre-filtered height map.
  glActiveTexture(GL_TEXTURE0_ARB);
  if ((texturing == 0) || (texturing == 1)) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureDotID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_DOT3_RGBA_EXT);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PRIMARY_COLOR_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
  } else if (texturing == 3) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureDotID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  } else {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }

  // Setup the second texture stage to modulate the previous dot product
  // and the the base texture.
  glActiveTexture(GL_TEXTURE1_ARB);
  if (texturing == 0) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);

    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_ALPHA);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
  } else if (texturing == 4) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  } else {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }
}

//////////////////////////
// Setup texture stages for Dot3 Bump-mapping with a Cube Map normalization.
//////////////////////////
void
Dot3CubeBump() {
  // Setup first texture stage to take the normal to the light vector
  // encoded as the texture coordinates and use a cube map to get an
  // interpolated normalized vector.
  glActiveTexture(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  if ((texturing == 0) || (texturing == 1) || (texturing == 2)) {
     glDisable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubemapID);
     glEnable(GL_TEXTURE_CUBE_MAP_ARB);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  } else {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }

  // Setup second texture stage to perform the dot product of the
  // first stage (a normalized vector to the light) and the pre-filtered
  // height map.
  glActiveTexture(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  if ((texturing == 0) || (texturing == 1)){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureDotID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_DOT3_RGB_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
  } else if (texturing == 3) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureDotID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  } else {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }

  // Setup third texture stage to modulate the lighting value from
  // the second stage with the base texture map to get the final
  // bump mapped output.
  glActiveTexture(GL_TEXTURE2_ARB);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  if (texturing == 0) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);

    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
  } else if (texturing == 4) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID[currentTexture]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  } else {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }
}

//////////////////////////
// Setup texture stages for 3D lightmaps.
//////////////////////////
void
lightmap3D(void) {
  // Load the lightmap as the first texture unit
  glActiveTexture(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glEnable(GL_TEXTURE_3D_EXT);
  glBindTexture(GL_TEXTURE_3D_EXT, lightmapID[0]);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Turn on texture generation to give each vertex it's world coordinates
  // as it's texture coordinates. We'll use a texture matrix to convert these
  // into something useful.
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glEnable(GL_TEXTURE_GEN_S);
  GLfloat sgenparams[] = {1.0f, 0.0f, 0.0f, 0.0f};
  glTexGenfv(GL_S, GL_OBJECT_PLANE, sgenparams);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glEnable(GL_TEXTURE_GEN_T);
  GLfloat tgenparams[] = {0.0f, 1.0f, 0.0f, 0.0f};
  glTexGenfv(GL_T, GL_OBJECT_PLANE, tgenparams);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  GLfloat rgenparams[] = {0.0f, 0.0f, 1.0f, 0.0f};
  glTexGenfv(GL_R, GL_OBJECT_PLANE, rgenparams);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);

  // Create a texture matrix to convert world space into lightmap space.
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity ();
  GLfloat sf = scaleFactor*0.00125f;
  glScalef(sf, sf, sf);
  GLfloat lp[3];
  vecMatMult(lightpos, lightMat, lp);
  glTranslatef (-lp[0], -lp[1], -lp[2]);
  glMatrixMode(GL_MODELVIEW);

  // Disable second texture unit
  glActiveTexture(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glDisable(GL_TEXTURE_3D_EXT);

  // Disable third texture unit
  glActiveTexture(GL_TEXTURE2_ARB);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glDisable(GL_TEXTURE_3D_EXT);
}

//////////////////////////
// Show the texturing flag label
//////////////////////////
void displayText(GLint textFlag) 
{
  switch (textFlag) {
    case 0:
      flDrawString(0.01, 0.01, "DOT3 Bump mapping");
      break;
    case 1:
      flDrawString(0.01, 0.01, "Normal dot Light Vector");
      break;
    case 2:
      flDrawString(0.01, 0.01, "Light Vector as color");
      break;
    case 3:
      flDrawString(0.01, 0.01, "Pre-Filtered Height Map");
      break;
    case 4:
      flDrawString(0.01, 0.01, "Base Map");
      break;
    case 5:
      flDrawString(0.01, 0.01, "Light Map");
      break;
    default:
      flDrawString(0.01, 0.01, "Unknown Setting");
      break;
  }
}

//////////////////////////
// Draw the object
//////////////////////////
void drawObject(GLuint cubeFlag) 
{
  // Determine light position in object space.
  GLfloat lp[3];
  vecMatMult(lightpos, lightMat, lp);
 
  // If we aren't doing wireframe then do each pass of the rendering.
  if (!wireframe) {
     // Render object. First pass for DOT3 bump mapping.
    glDisable(GL_BLEND);
    if (texturing < 5) {
      bumpObject.computeLighting(lp, cubeFlag);
      if (cubeFlag) {
        Dot3CubeBump();
      } else {
        Dot3Bump();
      }
      bumpObject.renderObject(cubeFlag, 0);
    }

    // Render object. Second pass for 3D lightmapping.
    if (texturing == 0) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
    }
    if ((texturing == 0) || (texturing == 5)) {
      lightmap3D();  
      bumpObject.renderObject(cubeFlag, 1);
    }
  }

  // Disable texture units so vectors and light appear correctly.
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glActiveTexture(GL_TEXTURE0_ARB);
     glMatrixMode(GL_TEXTURE);
     glLoadIdentity ();
     glMatrixMode(GL_MODELVIEW);
     glDisable(GL_TEXTURE_3D_EXT);
     glDisable(GL_TEXTURE_CUBE_MAP_ARB);
     glDisable(GL_TEXTURE_2D);
     glDisable(GL_TEXTURE_GEN_S);
     glDisable(GL_TEXTURE_GEN_T);
     glDisable(GL_TEXTURE_GEN_R);
  glActiveTexture(GL_TEXTURE1_ARB);
     glMatrixMode(GL_TEXTURE);
     glLoadIdentity ();
     glMatrixMode(GL_MODELVIEW);
     glDisable(GL_TEXTURE_3D_EXT);
     glDisable(GL_TEXTURE_CUBE_MAP_ARB);
     glDisable(GL_TEXTURE_2D);
     glDisable(GL_TEXTURE_GEN_S);
     glDisable(GL_TEXTURE_GEN_T);
     glDisable(GL_TEXTURE_GEN_R);
  glActiveTexture(GL_TEXTURE2_ARB);
     glMatrixMode(GL_TEXTURE);
     glLoadIdentity ();
     glMatrixMode(GL_MODELVIEW);
     glDisable(GL_TEXTURE_3D_EXT);
     glDisable(GL_TEXTURE_CUBE_MAP_ARB);
     glDisable(GL_TEXTURE_2D);
     glDisable(GL_TEXTURE_GEN_S);
     glDisable(GL_TEXTURE_GEN_T);
     glDisable(GL_TEXTURE_GEN_R);

  // Show wireframe if desired.
  if (wireframe) {
    bumpObject.computeLighting(lp, cubeFlag);
    glColor4fv (white);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    bumpObject.renderObject(GL_FALSE, 1);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }

  // Draw vectors and coordinate systems
  if (showVectors || showTanCoord) {
    glColor4fv (white);
    bumpObject.renderVectors(scaleFactor*2.0f, showTanCoord, showVectors, 
                             cubeFlag);
  }

  // Draw the light as a magenta sphere
  static GLfloat lightcolor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
  glColor4fv (lightcolor);
  static GLUquadricObj *sphere = NULL;
  if (sphere == NULL) {
    sphere = gluNewQuadric ();
  }
  glPushMatrix ();
     glTranslatef (lp[0], lp[1], lp[2]);
     gluSphere (sphere, (0.5f*scaleFactor), 10, 10);
  glPopMatrix ();

  // Draw the labeling text.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0f,1.0f,0.0f,1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glActiveTexture(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textID);
  glColor4fv (white);
  if (cubeFlag) {
    flDrawString(0.01, 0.9, "With Cube Map Normalizer");
  }
  displayText(texturing);
}

//////////////////////////
// Scene drawing function
//////////////////////////
void 
display(void) {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Viewport parameter calculation
  int w = width/2;
  int h = height;
  
  // Setup first viewport
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, (double)w/(double)h, clipNear, clipFar);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLoadMatrixf(viewMat);
  drawObject(GL_FALSE);

  // Setup second viewport
  glViewport(w,0,w,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, (double)w/(double)h, clipNear, clipFar);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLoadMatrixf(viewMat);
  drawObject(GL_TRUE);

  // Draw the dividing line.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0f,1.0f,0.0f,1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor4fv (white);
  glBegin(GL_LINES);
    glVertex3f (0.0f, 0.0f, 0.0f);
    glVertex3f (0.0f, 1.0f, 0.0f);
  glEnd();
  
  // Finish up
  glFinish ();
  glutSwapBuffers();
}

//////////////////////////
// Projection and View Setup
///////////////////////////
void 
MyReshape(int w,int h){
  width = w;
  height = h;
}

/////////////////////////////////////
// Generate the faces for the normalizer cubemap
/////////////////////////////////////
void
generateCubemap(void)
{
#define PackFloatInByte(in)  (GLubyte) ((((in)+1.0f) / 2.0f) * 255.0f)
  int x,y,z;
  GLfloat mapSize = (GLfloat)CUBEMAP_TEX_SIZE;
  GLfloat halfMapSize = (GLfloat)((int) (CUBEMAP_TEX_SIZE/2));
  printf("Generating cubemap\n");
  
  // Positive X face
  for (y = 0; y < mapSize; y++) {
    for (z = 0; z < mapSize; z++) {
      GLfloat fX = (GLfloat) (halfMapSize);
      GLfloat fY = (GLfloat) (halfMapSize - y);
      GLfloat fZ = (GLfloat) (halfMapSize - z);
      GLfloat oolen = 1.0f/sqrtf(fX*fX + fY*fY + fZ*fZ);
      fX *= oolen;  fY *= oolen;  fZ *= oolen;
      cubemapPX[y*3*CUBEMAP_TEX_SIZE + z*3 + 0] = PackFloatInByte(fX);
      cubemapPX[y*3*CUBEMAP_TEX_SIZE + z*3 + 1] = PackFloatInByte(fY);
      cubemapPX[y*3*CUBEMAP_TEX_SIZE + z*3 + 2] = PackFloatInByte(fZ);
    }
  }

  // Negative X face
  for (y = 0; y < mapSize; y++) {
    for (z = 0; z < mapSize; z++) {
      GLfloat fX = (GLfloat) (-halfMapSize);
      GLfloat fY = (GLfloat) (halfMapSize - y);
      GLfloat fZ = (GLfloat) (z - halfMapSize);
      GLfloat oolen = 1.0f/sqrtf(fX*fX + fY*fY + fZ*fZ);
      fX *= oolen;  fY *= oolen;  fZ *= oolen;
      cubemapNX[y*3*CUBEMAP_TEX_SIZE + z*3 + 0] = PackFloatInByte(fX);
      cubemapNX[y*3*CUBEMAP_TEX_SIZE + z*3 + 1] = PackFloatInByte(fY);
      cubemapNX[y*3*CUBEMAP_TEX_SIZE + z*3 + 2] = PackFloatInByte(fZ);
    }
  }

  // Positive Y face
  for (z = 0; z < mapSize; z++) {
    for (x = 0; x < mapSize; x++) {
      GLfloat fX = (GLfloat) (x - halfMapSize);
      GLfloat fY = (GLfloat) (halfMapSize);
      GLfloat fZ = (GLfloat) (z - halfMapSize);
      GLfloat oolen = 1.0f/sqrtf(fX*fX + fY*fY + fZ*fZ);
      fX *= oolen;  fY *= oolen;  fZ *= oolen;
      cubemapPY[z*3*CUBEMAP_TEX_SIZE + x*3 + 0] = PackFloatInByte(fX);
      cubemapPY[z*3*CUBEMAP_TEX_SIZE + x*3 + 1] = PackFloatInByte(fY);
      cubemapPY[z*3*CUBEMAP_TEX_SIZE + x*3 + 2] = PackFloatInByte(fZ);
    }
  }

  // Negative Y face
  for (z = 0; z < mapSize; z++) {
    for (x = 0; x < mapSize; x++) {
      GLfloat fX = (GLfloat) (x - halfMapSize);
      GLfloat fY = (GLfloat) (-halfMapSize);
      GLfloat fZ = (GLfloat) (halfMapSize - z);
      GLfloat oolen = 1.0f/sqrtf(fX*fX + fY*fY + fZ*fZ);
      fX *= oolen;  fY *= oolen;  fZ *= oolen;
      cubemapNY[z*3*CUBEMAP_TEX_SIZE + x*3 + 0] = PackFloatInByte(fX);
      cubemapNY[z*3*CUBEMAP_TEX_SIZE + x*3 + 1] = PackFloatInByte(fY);
      cubemapNY[z*3*CUBEMAP_TEX_SIZE + x*3 + 2] = PackFloatInByte(fZ);
    }
  }

  // Positive Z face
  for (y = 0; y < mapSize; y++) {
    for (x = 0; x < mapSize; x++) {
      GLfloat fX = (GLfloat) (x - halfMapSize);
      GLfloat fY = (GLfloat) (halfMapSize - y);
      GLfloat fZ = (GLfloat) (halfMapSize);
      GLfloat oolen = 1.0f/sqrtf(fX*fX + fY*fY + fZ*fZ);
      fX *= oolen;  fY *= oolen;  fZ *= oolen;
      cubemapPZ[y*3*CUBEMAP_TEX_SIZE + x*3 + 0] = PackFloatInByte(fX);
      cubemapPZ[y*3*CUBEMAP_TEX_SIZE + x*3 + 1] = PackFloatInByte(fY);
      cubemapPZ[y*3*CUBEMAP_TEX_SIZE + x*3 + 2] = PackFloatInByte(fZ);
    }
  }

  // Negative Z face
  for (y = 0; y < mapSize; y++) {
    for (x = 0; x < mapSize; x++) {
      GLfloat fX = (GLfloat) (halfMapSize - x);
      GLfloat fY = (GLfloat) (halfMapSize - y);
      GLfloat fZ = (GLfloat) (-halfMapSize);
      GLfloat oolen = 1.0f/sqrtf(fX*fX + fY*fY + fZ*fZ);
      fX *= oolen;  fY *= oolen;  fZ *= oolen;
      cubemapNZ[y*3*CUBEMAP_TEX_SIZE + x*3 + 0] = PackFloatInByte(fX);
      cubemapNZ[y*3*CUBEMAP_TEX_SIZE + x*3 + 1] = PackFloatInByte(fY);
      cubemapNZ[y*3*CUBEMAP_TEX_SIZE + x*3 + 2] = PackFloatInByte(fZ);
    }
  }

  // Setup texture environment and load cubemap.
  glGenTextures(1, &cubemapID);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubemapID);
  glEnable(GL_TEXTURE_CUBE_MAP_ARB);
  glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, 
                  GL_NEAREST);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0, 3, 
               CUBEMAP_TEX_SIZE, CUBEMAP_TEX_SIZE, 0, GL_RGB,
               GL_UNSIGNED_BYTE, cubemapPX);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 0, 3, 
               CUBEMAP_TEX_SIZE, CUBEMAP_TEX_SIZE, 0, GL_RGB,
               GL_UNSIGNED_BYTE, cubemapPY);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 0, 3, 
               CUBEMAP_TEX_SIZE, CUBEMAP_TEX_SIZE, 0, GL_RGB,
               GL_UNSIGNED_BYTE, cubemapPZ);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 0, 3, 
               CUBEMAP_TEX_SIZE, CUBEMAP_TEX_SIZE, 0, GL_RGB,
               GL_UNSIGNED_BYTE, cubemapNX);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 0, 3, 
               CUBEMAP_TEX_SIZE, CUBEMAP_TEX_SIZE, 0, GL_RGB,
               GL_UNSIGNED_BYTE, cubemapNY);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 0, 3, 
               CUBEMAP_TEX_SIZE, CUBEMAP_TEX_SIZE, 0, GL_RGB,
               GL_UNSIGNED_BYTE, cubemapNZ);
}

/////////////////////////////////////
// Generate the 3D lightmap. Note we only generate a quater of a sphere and
// use the mirror wrap mode to fill in the rest.
/////////////////////////////////////
void
generateLightmap3D(void)
{
  // Generate the lightmap texture
  printf("Generating lightmap\n");
#define SQ(A) ((A)*(A))
  float rad = 63.0f;
  float rad_sq = rad*rad;
  for (int r = 0; r < SIZE_LIGHTMAP_3D; r++) {
    for (int t = 0; t < SIZE_LIGHTMAP_3D; t++) {
      for (int s = 0; s < SIZE_LIGHTMAP_3D; s++) {
        float d_sq = SQ(r) + SQ(s) + SQ(t);
          if (d_sq < rad_sq) {
            float falloff = SQ((rad_sq - d_sq)/rad_sq);
            lightmap[r*SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*3 +
                        t*SIZE_LIGHTMAP_3D*3 + s*3 + 0] = (255.0f * falloff);
            lightmap[r*SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*3 +
                        t*SIZE_LIGHTMAP_3D*3 + s*3 + 1] = (255.0f * falloff);
            lightmap[r*SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*3 +
                        t*SIZE_LIGHTMAP_3D*3 + s*3 + 2] = (255.0f * falloff);
          } else {
            lightmap[r*SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*3 +
                        t*SIZE_LIGHTMAP_3D*3 + s*3 + 0] = 0;
            lightmap[r*SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*3 +
                        t*SIZE_LIGHTMAP_3D*3 + s*3 + 1] = 0;
            lightmap[r*SIZE_LIGHTMAP_3D*SIZE_LIGHTMAP_3D*3 +
                        t*SIZE_LIGHTMAP_3D*3 + s*3 + 2] = 0;
          }
        
      }
    }
  }

  // Setup texture environment and load cubemap.
  glGenTextures(NUM_LIGHTMAP_TEXTURES, lightmapID);
  glBindTexture(GL_TEXTURE_3D_EXT, lightmapID[0]);
  glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S, 
                  GL_MIRROR_CLAMP_TO_EDGE_ATI);
  glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T,
                  GL_MIRROR_CLAMP_TO_EDGE_ATI);
  glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R_EXT,
                  GL_MIRROR_CLAMP_TO_EDGE_ATI);
  glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage3DEXT(GL_TEXTURE_3D_EXT, 0, 3, 
                  SIZE_LIGHTMAP_3D, SIZE_LIGHTMAP_3D, SIZE_LIGHTMAP_3D,
                  0, GL_RGB, GL_UNSIGNED_BYTE, lightmap);
}

/////////////////////////////////////
// Setup GL textures.
/////////////////////////////////////
void
initTextures(void)
{
  // Reference to global error message.
  extern char gszErrMsg[];
  gszErrMsg[0] = '\0';

  // Initialize the text texture
  printf("Loading: charset.tga\n");
  glGenTextures(1, &textID);
  charset = new ATI_GLTexture("charset.tga", textID, 
                            GL_TRUE, GL_RGBA, GL_RGBA);
  if (gszErrMsg[0] != '\0') printf(gszErrMsg);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glEnable(GL_TEXTURE_2D);
  flSetWidth(1.0f/25.0f);
  flSetHeight(1.0f/10.0f);

  // Load the set of textures needed.
  glGenTextures(NUM_DEMO_TEXTURES, textureID);
  glGenTextures(NUM_DEMO_TEXTURES, textureDotID);
  for (GLuint i=0; i < NUM_DEMO_TEXTURES; i++) {
    // Load Base texture
    gszErrMsg[0] = '\0';
    printf("Loading: %s\n", textureName[i]);
    texture[i] = new ATI_GLTexture(textureName[i], textureID[i],
                                   GL_TRUE, GL_RGBA, GL_RGBA);
    if (gszErrMsg[0] != '\0') printf("Error loading %s: %s\n",
                                     textureName[i],gszErrMsg);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    gszErrMsg[0] = '\0';

    // Load pre-filtered height maps
    printf("Loading: %s\n", textureDotName[i]);
    textureDot[i] = new ATI_GLTexture(textureDotName[i], textureDotID[i],
                                      GL_TRUE, GL_RGBA, GL_RGBA);
    if (gszErrMsg[0] != '\0') printf("Error loading %s: %s\n",
                                     textureDotName[i],gszErrMsg);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
  }

  // Handle cubemap
  generateCubemap();

  // Handle 3D lightmaps
  generateLightmap3D();

  // If anisotropy is supported, use it.
  if (anisotropic) {
    GLfloat maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
  }
}

///////////////////////////
// GL Initialization
///////////////////////////
void 
myinit(void){
  // Basic GL setup
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glShadeModel (GL_SMOOTH);
  glEnable(GL_CULL_FACE);
  glCullFace (GL_BACK);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

  // Load up our model.
  printf("Loading: %s\n", pofName);
  bumpObject.loadObject(pofName);

  // Based on the bounding box set the initial transform.
  float bbox[6];
  bumpObject.getBBox(bbox);
  GLfloat cx = (bbox[3] - bbox[0])/2.0f + bbox[0];
  GLfloat cy = (bbox[4] - bbox[1])/2.0f + bbox[1];
  GLfloat cz = (bbox[5] - bbox[2])/2.0f + bbox[2];
  GLfloat dx = bbox[0] - cx;
  GLfloat dy = bbox[1] - cy;
  GLfloat dz = bbox[2] - cz;
  GLfloat r = sqrt(dx*dx + dy*dy + dz*dz);
  GLfloat ang = (float)(tan((double)((3.141592f/180.0f)*(fov/2.0f))));
  GLfloat dist;
  if ((ang > -0.0000001) && (ang < 0.0000001)) { // Don't divide by zero
    dist = 500.0f;
  } else {
    dist = (r * 1.2f) / ang;
  }
  GLfloat zf = r * 2.2f + fabs(cz);
  if (zf > clipFar) clipFar = zf;
  matrixIdentity(viewMat);
  viewMat[12] = -cx;
  viewMat[13] = -cy;
  viewMat[14] = -(cz + dist);

  // Lighting parameters and scale factors.
  matrixIdentity(lightMat);
  lightpos[2] = lightDist = cz + (r*0.6);
  scaleFactor = dist * 0.03;

  // Texturing
  initTextures();
}

/////////////////////////////////////
// Key input handler
//   key - Which special key pressed
/////////////////////////////////////
void 
KeyFunc(unsigned char key,int x,int y){
  switch(key){
    case '\x1b': // Quit
    case 'Q':
    case 'q':
      exit(0);
      break;
    case 't': // Switch current texture
    case 'T':
      currentTexture = (currentTexture + 1) % NUM_DEMO_TEXTURES;
      break;
    case ' ':
      moving = !moving;
      break;
    case 'v':
    case 'V':
      showVectors = !showVectors;
      break;
    case 'c':
    case 'C':
      showTanCoord = !showTanCoord;
      break;
    case 'm':
    case 'M':
      texturing = (texturing + 1) % 6;
      break;
    case 'w':
    case 'W':
      wireframe = !wireframe;
      break;
  };
  glutPostRedisplay();
}

/////////////////////////////////////
// Timer callback
/////////////////////////////////////
void 
TimerFunc(int value){
  // Move the light around if the user requested that.
  if (moving && !rightDown) {
    glPushMatrix();
       glLoadIdentity();
       glMultMatrixf(lightMat);
       glRotatef(2.0f, 1.0f, 1.0f, 0.0f);
       glGetFloatv(GL_MODELVIEW_MATRIX, lightMat);
    glPopMatrix();
    display();
  }
  glutTimerFunc(100,TimerFunc,0);
}

/////////////////////////////////////
// Handle special case key presses
//   key - Which special key was pressed
/////////////////////////////////////
void 
SpecialKeys(int key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_LEFT:
    break;
  case GLUT_KEY_RIGHT:
    break;
  case GLUT_KEY_UP:
    // Move light further from object center
    lightpos[2] += (lightDist * 0.01f);
    break;
  case GLUT_KEY_DOWN:
    // Move light closer to object center
    lightpos[2] -= (lightDist * 0.01f);
    break;
  }
  glutPostRedisplay();
}

/////////////////////////////////////
// Handle mouse movement.
//   x - current x position.
//   y - current y position.
/////////////////////////////////////
void 
MouseMove(int x, int y)
{
  // Left button down so move the object.
  if (leftDown) {
    glMatrixMode(GL_MODELVIEW);
    GLfloat m[16];
    matrixInvert(viewMat, m);
    GLfloat v[3] = {(y - leftY) * 0.3f, (x - leftX) * 0.3f, 0.0f };
    GLfloat o[3];
    vecMat3x3Mult(v, m, o);
    glPushMatrix();
       glLoadIdentity();
       glMultMatrixf(viewMat);
       glRotatef(o[0], 1.0f, 0.0f, 0.0f);
       glRotatef(o[1], 0.0f, 1.0f, 0.0f);
       glRotatef(o[2], 0.0f, 0.0f, 1.0f);
       glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);
    glPopMatrix();
    leftX = x;
    leftY = y;
    glutPostRedisplay();
  }

  // Right button down so move the light.
  if (rightDown) {
    glMatrixMode(GL_MODELVIEW);
    GLfloat m1[16];
    GLfloat m2[16];
    matrixInvert(viewMat, m1);
    matrixInvert(lightMat, m2);
    GLfloat v[3] = {(y - rightY) * 0.3f, (x - rightX) * 0.3f, 0.0f };
    GLfloat o[3];
    vecMat3x3Mult(v, m1, o);
    vecMat3x3Mult(o, m2, v);
    glPushMatrix();
       glLoadIdentity();
       glMultMatrixf(lightMat);
       glRotatef(v[2], 0.0f, 0.0f, 1.0f);
       glRotatef(v[1], 0.0f, 1.0f, 0.0f);
       glRotatef(v[0], 1.0f, 0.0f, 0.0f);
       glGetFloatv(GL_MODELVIEW_MATRIX, lightMat);
    glPopMatrix();
    rightX = x;
    rightY = y;
    glutPostRedisplay();
  }
}
/////////////////////////////////////
// Handle mouse button presses/releases.
//   button - The button pressed.
//   state - Was the button pressed or release.
//   x - current x position.
//   y - current y position.
/////////////////////////////////////
void 
MouseUpDown(int button, int state, int x, int y) 
{
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      leftDown = GL_TRUE;
      leftX = x;
      leftY = y;
    }
    else if (state == GLUT_UP) {
      leftDown = GL_FALSE;
    }
  }
  if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) {
      rightDown = GL_TRUE;
      rightX = x;
      rightY = y;
    }
    else if (state == GLUT_UP) {
      rightDown = GL_FALSE;
    }
  }
}

/////////////////////////////////////
// Determine if a particular extension is supported
//   extension -- Name of the extension to check
/////////////////////////////////////
int 
extensionSupported(const char *extension)
{
  static const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;

  where = (GLubyte *) strchr(extension, ' ');
  if (where || *extension == '\0')
    return 0;

  if (!extensions)
    extensions = glGetString(GL_EXTENSIONS);

  start = extensions;
  for (;;) {
    where = (GLubyte *) strstr((const char *) start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if (where == start || *(where - 1) == ' ') {
      if (*terminator == ' ' || *terminator == '\0') {
        return 1;
      }
    }
    start = terminator;
  }
  return 0;
}

/////////////////////////////////////
// Check to make sure the extensions we need are available.
/////////////////////////////////////
void
checkExtensions() {
  if (!extensionSupported("GL_ARB_multitexture")){
    printf("No GL_ARB_multitexture support!\n");
    Sleep(5000);
    exit(0);
  }
  glActiveTexture = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
  if (glActiveTexture == NULL) {
    printf("No glActiveTexture function available\n");
    Sleep(5000);
    exit(0);
  }
  glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
  if (glClientActiveTexture == NULL) {
    printf("No glClientActiveTexture function available\n");
    Sleep(5000);
    exit(0);
  }
  if (!extensionSupported("GL_EXT_texture_env_combine")){
    printf("No GL_EXT_texture_env_combine support!\n");
    Sleep(5000);
    exit(0);
  }
  if (!extensionSupported("GL_ARB_texture_cube_map")){
    printf("No GL_ARB_texture_cube_map support!\n");
    Sleep(5000);
    exit(0);
  }
  if (!extensionSupported("GL_EXT_texture_env_dot3")){
    printf("No GL_EXT_texture_env_dot3 support!\n");
    Sleep(5000);
    exit(0);
  }
  if (!extensionSupported("GL_EXT_texture3D")){
    printf("No GL_EXT_texture3D support!\n");
    Sleep(5000);
    exit(0);
  }
  glTexImage3DEXT = (PFNGLTEXIMAGE3DEXT)wglGetProcAddress("glTexImage3DEXT");
  if (glTexImage3DEXT == NULL) {
    printf("No glTexImage3DEXT function available\n");
    Sleep(5000);
    exit(0);
  }
  if (!extensionSupported("GL_EXT_texture_filter_anisotropic")){
    anisotropic = GL_FALSE;
  }
}

/////////////////////////////////////
// Entry point
/////////////////////////////////////
void 
main(int argc,char **argv){
  // If the user specified a file name use that instead.
  if (argc == 2) {
    pofName = argv[1];
  } else {
    pofName = "crackbox.pof";
  }

  printf("Radeon Dot3 Bump Demo\n");
  printf("June 26, 2000\n");
  printf(" m             - Show stages of the bumpmapping process\n");
  printf(" t             - Cycle through textures\n");
  printf(" v             - Toggle vector drawing\n");
  printf(" c             - Toggle display of tangent coordinate system\n");
  printf(" w             - Toggle wireframe\n");
  printf(" UP ARROW      - Move light source further from object center\n");
  printf(" DOWN ARROW    - Move light source closer to object center\n");
  printf(" LEFT + MOUSE  - Rotate Object\n");
  printf(" RIGHT + MOUSE - Rotate Light\n");
  printf(" SPACE         - Toggle Motion\n");
  printf(" ESC           - Quit program\n");
  printf("\n");

  // Create window.
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutCreateWindow("DOT3 Bump Demo");

  // Check extensions.
  checkExtensions();

  // Setup GLUT callbacks.
  glutDisplayFunc(display);
  glutReshapeFunc(MyReshape);
  glutKeyboardFunc(KeyFunc);
  glutSpecialFunc(SpecialKeys);
  glutMotionFunc(MouseMove);
  glutMouseFunc(MouseUpDown);
  myinit();
  glutTimerFunc(100,TimerFunc,0);
  glutMainLoop();
}
