/* Copyright (C) David Paull, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) David Paull, 2000"
 */
#include "glut.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "mtxlib.h"
#include "demolib.h"
#include "spinner.h"
#include "vectorcamera.h"


////
// Globals
//
int 		gFrameNumber = 0;
vector3 	gCameraPos;
vector3 	gLookatPos;
const float 		gKeyStep = 10.0f;
const unsigned int	gWindowWidth  = 640;
const unsigned int	gWindowHeight = 480;
Spinner		s;

////
// Function Prototypes
//
void DisplayFunc(void);
void KeyboardFunc(unsigned char key, int x, int y);
void KeyboardSpecialFunc(int key, int x, int y);
void SetPerspectiveCamera(void);
void SetOrthographicCamera(void);
void IdleFunc(void);
void ReshapeFunc(int w, int h);
void InitializeProgram(void);


////
// Functions
//


// main()
//
int main(int argc, char* argv[]) {

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowPosition(200, 0);
  glutInitWindowSize(gWindowWidth, gWindowHeight);
  glutCreateWindow("GPG Demo App");
  
  glutDisplayFunc(DisplayFunc);
  glutKeyboardFunc(KeyboardFunc);
  glutSpecialFunc(KeyboardSpecialFunc);
  glutReshapeFunc(ReshapeFunc);
  glutIdleFunc(IdleFunc);
  
  InitializeProgram();
  
  glutSwapBuffers();
  glutMainLoop();
  
  return 0;
}


// StringLength()
//
// Returns the string length using font "font".
//
unsigned int StringLength(void* font, const char* str) {
  int stringSize = 0;
  const char* c;

  for (c=str; *c != '\0'; c++)
    stringSize += glutBitmapWidth(font, *c);
  
  return stringSize;
}


// DisplayString()
//
// Display a string using a specific font at the location (x,y)
//   on the screen.
//
void DisplayString(int x, int y, void* font, const char* str) {
  const char* c;

  // Position the cursor for font drawing
  glRasterPos2i(x, y);

  // Draw the font letter-by-letter
  for (c=str; *c != '\0'; c++)
    glutBitmapCharacter(font, *c);
}


// DrawBox()
//
// Draw a 2D box at the specified coordinates.
//
bool UseVectorCamera = false;
void DrawBox(float ulx, float uly, float lrx, float lry) {

  glBegin(GL_QUADS);
  glVertex2f(ulx, uly);
  glVertex2f(ulx, lry);
  glVertex2f(lrx, lry);
  glVertex2f(lrx, uly);
  glEnd();
}


// DisplayFunc()
//
void DisplayFunc(void) {
  char infoString[256];
  char toomuchinfoString[256];
  char quitString[] = "<ESC> to Quit";
  int  stringSize;

  // Clear the frame buffer and Z buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // Set up a Perspective Projection camera
  SetPerspectiveCamera();

  // Set up current camera
  gluLookAt(gCameraPos.x, gCameraPos.y, gCameraPos.z, 
	    gLookatPos.x, gLookatPos.y, gLookatPos.z, 
	    0, 1, 0);  // Up vector


  vcLookAt(gCameraPos.x, gCameraPos.y, gCameraPos.z, 
	    gLookatPos.x, gLookatPos.y, gLookatPos.z, 
	    0, 1, 0);  // Up vector


  // Draw some 3D 
  glBlendFunc(GL_ONE, GL_ZERO);

  // Draw the spinner
  if(UseVectorCamera)
  {
	SetOrthographicCamera();
  	s.Display();
  }
  else
  {
   	s.Display();
  }
  // Set up an Orthographic Perspective camera

  SetOrthographicCamera();
  // Draw some 2D stuff
  //

  // Draw a box for the info string
  if(UseVectorCamera)
  {
    sprintf(infoString,"VectorCam");
  }
  else
  {
    sprintf(infoString,"OpenGL");
  }
  stringSize = StringLength(GLUT_BITMAP_TIMES_ROMAN_24, infoString);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4ub(0x0f, 0x3f, 0xff, 0x7f);
  DrawBox(0, 0, 20+stringSize, 34);

  // Display the info string
  glBlendFunc(GL_ONE, GL_ZERO);
  glColor4ub(0xff, 0xff, 0xff, 0x7f);
  DisplayString(10, 10, 
		GLUT_BITMAP_TIMES_ROMAN_24, infoString);  

  // Draw a box for the quit string
  stringSize = StringLength(GLUT_BITMAP_TIMES_ROMAN_24, quitString);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4ub(0xff, 0x3f, 0x0f, 0x7f);
  DrawBox(gWindowWidth-20-stringSize, 0, gWindowWidth, 34);

  // Display the quit string
  glBlendFunc(GL_ONE, GL_ZERO);
  glColor4ub(0xff, 0xff, 0xff, 0x7f);
  DisplayString(gWindowWidth-10-stringSize, 10, 
		GLUT_BITMAP_TIMES_ROMAN_24, quitString);


  if(UseVectorCamera)
  {
	  // Display the info string
      sprintf(toomuchinfoString,"same result, less math.");
	  glBlendFunc(GL_ONE, GL_ZERO);
	  glColor4ub(0xff, 0xff, 0xff, 0x7f);
	  DisplayString(10, gWindowHeight-20, 
			GLUT_BITMAP_TIMES_ROMAN_24, toomuchinfoString);  
  }
  else
  {
	  // Display the info string
      sprintf(toomuchinfoString,"see text, or code for description.");
	  glBlendFunc(GL_ONE, GL_ZERO);
	  glColor4ub(0xff, 0xff, 0xff, 0x7f);
	  DisplayString(10, gWindowHeight-20, 
			GLUT_BITMAP_TIMES_ROMAN_24, toomuchinfoString);  
      sprintf(toomuchinfoString,"'t' toggles.");
	  glBlendFunc(GL_ONE, GL_ZERO);
	  glColor4ub(0xff, 0xff, 0xff, 0x7f);
	  DisplayString(10, gWindowHeight-40, 
			GLUT_BITMAP_TIMES_ROMAN_24, toomuchinfoString);  
  }

  // Swap frame buffers
  glutSwapBuffers();

  // Keep a frame count
  gFrameNumber++;

}


// KeyboardFunc()
//
// This functions handles regular keyboard keys, including the
//   "ESCAPE" key, used to quit the app.
//
void KeyboardFunc(unsigned char key, int x, int y) {
  switch (key) {
  case 27: 
    exit(0);
  case 13: 
    break;
  case 't':
  case 'T': // Toggle 
	UseVectorCamera = !UseVectorCamera;
    break;
  }
}


// KeyboardSpecialFunc()
//
// This function handles the arrow keys.
//
void KeyboardSpecialFunc(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    gCameraPos.x += gKeyStep;
    break;
  case GLUT_KEY_DOWN:
    gCameraPos.x -= gKeyStep;
    break;
  case GLUT_KEY_LEFT:
    gCameraPos.y += gKeyStep;
    break;
  case GLUT_KEY_RIGHT:
    gCameraPos.y -= gKeyStep;
    break;
  }
}


// SetPerspectiveCamera()
//
// Sets up a perspective (3D) projection
//
void SetPerspectiveCamera(void) {
  // Enable Z buffer
  glEnable(GL_DEPTH_TEST);

  // Set the projection matrix
  // Two ways shown here-- with the OpenGL routine or the mtxlib
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#if 0
  glFrustum (-1.0, 1.0, -0.813949, 0.813949, 1, 512);
#else
  float fAspectRatio,Hfrac,Vfrac;
  #define FOV 1.5707963267948966192313216916398
  if(gWindowHeight>gWindowWidth)
  {
  	//////////////////////////////////
  	//// Tall And Skinny Viewport ////
  	//////////////////////////////////
  	fAspectRatio = (float)gWindowWidth/(float)gWindowHeight;
  	Hfrac =  tan(FOV*0.5*fAspectRatio);
  	Vfrac =  tan(FOV*0.5);
  }
  else
  {
  	//////////////////////////////
  	//// Normal Wide Viewport ////
  	//////////////////////////////
  	fAspectRatio = (float)gWindowHeight/(float)gWindowWidth;
  	Hfrac =  tan(FOV*0.5);
  	Vfrac =  tan(FOV*0.5*fAspectRatio);
  }

  matrix44 perspMatrix = FrustumMatrix44(-Hfrac,Hfrac, -Vfrac, Vfrac, 1, 512);
  glLoadMatrixf((float*)&perspMatrix);
#endif
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

#if 1
  // Turn backface culling off
  glDisable(GL_CULL_FACE);
#else
  // Turn backface culling on
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
#endif

}


// SetOrthographicCamera()
//
// Sets up an orthographic (2D) projection
//
void SetOrthographicCamera(void) {
  // Disable Z buffer
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, gWindowWidth, 0, gWindowHeight, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Disable backface culling
  glDisable(GL_CULL_FACE);
}


// IdleFunc()
//
void IdleFunc(void) {
  // Call the display routine next time through the main loop  
  glutPostRedisplay();

  // Call the routine to update spinner graphics
  s.Update();

}


// ReshapeFunc()
//
// This function is called when the user changes the window size.
//
void ReshapeFunc(int w, int h) {
  // Modify the viewport for the new window settings
  glViewport (0, 0, w, h);
}


// InitializeProgram()
//
// Set up some global states.
//
void InitializeProgram(void) {

  // Enable Gouraud shading
  glShadeModel(GL_SMOOTH);

  // Set clear color and alpha
  glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

  // Enable blending of source pixel data with frame buffer
  glEnable(GL_BLEND);

  // Render front faces filled
  glFrontFace(GL_CCW);
  glPolygonMode(GL_FRONT, GL_FILL);

  // Initialize camera position
  gCameraPos = vector3(0, 0, -1);
  gLookatPos = vector3(0, 0, 0);

  // Initialize the spinner
  s.Init();

}

