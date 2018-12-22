/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
//////////////////////////////////////////////////////////////
//
// Filename: main.cpp
//
// Author: Steve Rabin
// E-mail: stevera@noa.nintendo.com
// From the book "Game Programming Gems"
// From the article "Designing a General Robust AI Engine"
//
// Brief Disclaimer:
// This code is free to use for commercial use and is in the
// public domain. You may distribute, copy, modify, or use as
// is as long as this information is present. This code makes
// no guarantees written or implied and is provided solely as
// an example. Have a nice day!
//
// Purpose:
// This is where it all starts.
//
//////////////////////////////////////////////////////////////

//
// includes
//

#include <iostream.h>
#include <time.h>

#include "custom_time.h"
#include "game_object.h"
#include "game_object_db.h"
#include "fsm.h"
#include "msgroute.h"
#include "hud.h"

#define GAME_SPEED 40.0f


void Shutdown (void);

// GLUT stuff

#include "glut.h"
#include "mtxlib.h"
#include "time.h"

// GLUT globals

int 		gFrameNumber = 0;
vector3 	gCameraPos, gLookatPos;

const float 		   gKeyStep      = 2.0f;
const unsigned int	gWindowWidth  = 640;
const unsigned int	gWindowHeight = 480;

// GLUT function prototypes

void DisplayFunc (void);
void IdleFunc (void);
void Initialize3D (void);
void KeyboardFunc (unsigned char key, int x, int y);
void KeyboardSpecialFunc (int key, int x, int y);
void ReshapeFunc (int w, int h);
void SetPerspectiveCamera (void);
void SetOrthographicCamera (void);


// main
// Makes it go.

int main (void)
{

   InitTime();


   // initialize the GL code

   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowPosition(200, 0);
   glutInitWindowSize(gWindowWidth, gWindowHeight);
   glutCreateWindow("AI Engine App");

   glutDisplayFunc(DisplayFunc);
   glutKeyboardFunc(KeyboardFunc);
   glutSpecialFunc(KeyboardSpecialFunc);
   glutReshapeFunc(ReshapeFunc);
   glutIdleFunc(IdleFunc);
  
   Initialize3D();

   HUDInitScreen();

   //AI Game Object Creation
   InitDelayedMessages();
   {
	   int i;
	   GameObject* go;
	   char name[256];

	   for( i=0; i<40; i++ ) {
		   sprintf( name, "drone%d", i );
		   go = GODBCreateAndReturnGO( name );
		   FSMInitialize( go->unique_id, FSM_Drone );
	   }

   }

   glutSwapBuffers();
   glutMainLoop();


   return (0);
}

// Shutdown.
// Dumps out all the data about the final positions
// of the boids and then shuts down the demo.

void Shutdown (void)
{

//   exit(0);
}

/////////////////
// GLUT Functions
/////////////////

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
// Handles the display window

void DisplayFunc(void) 
{

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

   // Draw the boids
   GODBDraw();
   
   // Draw the HUD
   HUDDrawScreen();

   // Swap frame buffers

   glutSwapBuffers();

   // Keep a frame count

   gFrameNumber++;

}

// KeyboardFunc()
// Looks for the "ESCAPE" key, by which we quit the app

void KeyboardFunc(unsigned char key, int x, int y) 
{

   switch (key) 
   {
      case 27: 
         Shutdown();
         break;
   }
}

// KeyboardSpecialFunc()
// This function handles the arrow keys to move the camera around.

void KeyboardSpecialFunc(int key, int x, int y) 
{
   switch (key) 
   {
   case GLUT_KEY_UP:
      gCameraPos.z -= gKeyStep;
      break;
   case GLUT_KEY_DOWN:
      gCameraPos.z += gKeyStep;
      break;
   case GLUT_KEY_LEFT:
      gCameraPos.x -= gKeyStep;
      break;
   case GLUT_KEY_RIGHT:
      gCameraPos.x += gKeyStep;
      break;
   case GLUT_KEY_F1:
      gCameraPos.y += gKeyStep;
      break;
   case GLUT_KEY_F2:
      gCameraPos.y -= gKeyStep;
      break;
   case GLUT_KEY_F3:
      gLookatPos.x += gKeyStep;
      break;
   case GLUT_KEY_F4:
      gLookatPos.x -= gKeyStep;
      break;
   case GLUT_KEY_F5:
      gLookatPos.y += gKeyStep;
      break;
   case GLUT_KEY_F6:
      gLookatPos.y -= gKeyStep;
      break;
   case GLUT_KEY_F7:
      gLookatPos.z += gKeyStep;
      break;
   case GLUT_KEY_F8:
      gLookatPos.z -= gKeyStep;
      break;
   case GLUT_KEY_F9:
      break;
   case GLUT_KEY_F10:
      break;
   case GLUT_KEY_F11:
      break;
   case GLUT_KEY_F12:
      break;
   }
}

// Initialize3D()
// Finishes setting up the GLUT library.

void Initialize3D (void) 
{

   // Enable Gouraud shading

   glShadeModel(GL_SMOOTH);

   // Set clear color and alpha

   glClearColor(0.1f, 0.2f, 0.2f, 0.2f);

   // Enable blending of source pixel data with frame buffer

   glEnable(GL_BLEND);

   // Render front faces filled

   glFrontFace(GL_CCW);
   glPolygonMode(GL_FRONT, GL_FILL);

   // Initialize camera and lookat positions

   //gCameraPos = vector3(0.0, Box1->GetBoxHeight()*3, Box1->GetBoxHeight() * 2);
   gCameraPos = vector3(0.0f, 0.0f, 100.0f);
   gLookatPos = vector3(0.0f, 0.0f, 0.0f);

}

// IdleFunc()
// Idle function for between frames
void IdleFunc(void) 
{

   float deltaTime;

   // Mark time
   MarkTimeThisTick();
   deltaTime = GetElapsedTime() * GAME_SPEED;

   // Call the display routine next time through the main loop  

   glutPostRedisplay();

   HUDClearScreen();
   HUDPrintToScreen( "There is nothing to see but these text strings!!!\n\n" );
   HUDPrintToScreen( "This app shows a bare minimum AI engine implemented.\n" );
   HUDPrintToScreen( "Below you'll see the states of forty different game\n" );
   HUDPrintToScreen( "objects as they wander around in space. See if you\n" );
   HUDPrintToScreen( "can find a pattern. Then check the code to see if\n" );
   HUDPrintToScreen( "you're right. Hint: Great AI makes the player believe\n" );
   HUDPrintToScreen( "that the game is smarter than it really is.\n\n\n" );

   //Send any messages that have been waiting
   SendDelayedMessages();

   // Update all game objects in the AI engine
   GODBUpdate();

   GODBOutputStateInfoToHUD();

}

// ReshapeFunc()
// This function is called when the user changes the window size.

void ReshapeFunc(int w, int h) 
{

  // Modify the viewport for the new window settings

  glViewport (0, 0, w, h);

}

// SetPerspectiveCamera()
// Sets up a perspective (3D) projection

void SetPerspectiveCamera(void) 
{
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
// Sets up an orthographic (2D) projection

void SetOrthographicCamera(void) 
{

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

