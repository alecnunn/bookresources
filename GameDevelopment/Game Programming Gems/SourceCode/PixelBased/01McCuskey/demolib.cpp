/* Copyright (C) Mason McCuskey, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Mason McCuskey, 2000"
 */
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "mtxlib.h"
#include "demolib.h"
#include "3dsprite.h"

////
// Globals
//
int 		gFrameNumber = 0;
vector3 	gCameraPos;
vector3 	gLookatPos;
const float 		gKeyStep = 10.0f;
const unsigned int	gWindowWidth  = 640;
const unsigned int	gWindowHeight = 480;
C3DSprite	s;

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
#ifdef _WIN32
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, int nCmdShow)
#else
int main() 
#endif
{

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowPosition(200, 0);
  glutInitWindowSize(gWindowWidth, gWindowHeight);
  glutCreateWindow("Using 3D Hardware for 2D Sprite Effects");
  
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
  char infoString[] = "Example App";
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

  // Draw some 3D 
  glBlendFunc(GL_ONE, GL_ZERO);

  // Draw the sprite
  s.Display();

  // Set up an Orthographic Perspective camera
  SetOrthographicCamera();

  // Draw some 2D stuff
  //

  // Draw a box for the info string
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
  /*case 'w': // Up
  case 'W':
    gCameraPos.x += gKeyStep;
    break;
  case 's': // Down
  case 'S':
    gCameraPos.x -= gKeyStep;
    break;
  case 'a': // Left
  case 'A':
    gCameraPos.y -= gKeyStep;
    break;
  case 'd': // Right
  case 'D':
    gCameraPos.y += gKeyStep;
    break;
  case 'q':
  case 'Q':
    gCameraPos.z += gKeyStep;
    break;
  case 'e':
  case 'E':
    gCameraPos.z -= gKeyStep;
    break;
  case 't':
  case 'T': // Toggle 
    break;
    */
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
  glFrustum (-1.6, 1.6, -1.6, 1.6, 4, 512);
#else
  matrix44 perspMatrix = FrustumMatrix44(-1.6, 1.6, -1.6, 1.6, 4, 512);
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
  glOrtho(0, 640, 0, 320, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Disable backface culling
  glDisable(GL_CULL_FACE);
}


// IdleFunc()
//
void IdleFunc(void) {
  static int velocityx = 1;
  static int velocityy = 1;
  static int alphavel = 1;
  static int widthvel = 1;
  static int heightvel = 1;
  static int rotvel = 5;

  // Call the display routine next time through the main loop  
  glutPostRedisplay();

  // Call the routine to update sprite graphics
  s.SetX(s.GetX()+velocityx);
  s.SetY(s.GetY()+velocityy);
  s.SetAlpha(s.GetAlpha()+alphavel);
  s.SetWidth(s.GetWidth()+widthvel);
  s.SetHeight(s.GetHeight()+heightvel);
  s.SetRotation(s.GetRotation()+rotvel);

  if (s.GetX() > 100  || s.GetX() < -100) velocityx *= -1;
  if (s.GetY() > 80   || s.GetY() < -80) velocityy *= -1;
  if (s.GetAlpha() > 254 || s.GetAlpha() < 1) alphavel *= -1;
  if (s.GetWidth() > 100 || s.GetWidth() < 10) widthvel *= -1;
  if (s.GetHeight() > 100 || s.GetHeight() < 10) heightvel *= -1;
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
  gCameraPos = vector3(0,0,200);
  gLookatPos = vector3(0,0,0);

  // Initialize the sprite
  s.Init();

}

