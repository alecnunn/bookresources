/* Copyright (C) Steven Woodcock, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2001"
 */
//*********************************************************************
// Name:     main.cpp
// Purpose:  Driver routine for the Flocking With Teeth (Gems 2)
//           demonstration.  An OpenGL application pops up to show the 
//           user what's going on, and an output file is generated to the 
//           Windows desktop if various DEBUG options are specified 
//           (see readme.txt).
//*********************************************************************

//
// includes
//

#include <windows.h>
#include <iostream.h>
#include <time.h>
#include <mmsystem.h>

// flocking stuff

#include "CBox.h"
#include "CBoid.h"
#include "CFlock.h"
#include "CObstacle.h"

CBox   *Box1;
CFlock *Flocks[MaxFlocks];
CBoid  *Flies[MaxFlies];
CBoid  *Sparrows[MaxSparrows];
CBoid  *Hawks[MaxHawks];
CObstacle *Obstacles[MaxObstacles];

// flocking debug globals

bool  gDrawAxes           = FALSE;
bool  gDrawPreyLine       = TRUE;
bool  gDrawPerceptionDist = FALSE;
bool  gDrawKeepawayDist   = FALSE;
bool  gDrawSeparationDist = FALSE;

// flocking framerate control variables

float gFrameRate         = 20.0f;
float gMilliSecsPerFrame = 1000.0f/gFrameRate;
int   gLastTime          = 0;

// flocking function prototypes

void Shutdown (void);

// GLUT stuff

#include "glut.h"
#include "mtxlib.h"

// GLUT globals

int 		gFrameNumber = 0;
vector3 	gCameraPos, gLookatPos;

const float 		      gKeyStep      = 2.0f;
const unsigned int	   gWindowWidth  = 640;
const unsigned int	   gWindowHeight = 480;

// GLUT function prototypes

void DisplayFunc (void);
void IdleFunc (void);
void Initialize3D (void);
void KeyboardFunc (unsigned char key, int x, int y);
void KeyboardSpecialFunc (int key, int x, int y);
void ReshapeFunc (int w, int h);
void SetPerspectiveCamera (void);
void SetOrthographicCamera (void);

/////////////////////
// Flocking Functions
/////////////////////

// main
// Makes it go.

int main (void)
{

   int i;

   // initialize the RNG

   srand ((unsigned) time (NULL));

   // build a world for our boids to flock around in

   Box1 = new CBox (50.0, 50.0, 50.0);

   // build some obstacles in said world

   Obstacles[0] = new CObstacle (35.0, 5.0,  20.0,  20.0);
   Obstacles[1] = new CObstacle (25.0, 3.0,  10.0,  15.0);
   Obstacles[2] = new CObstacle (25.0, 3.0,  15.0,  10.0);
   Obstacles[3] = new CObstacle (35.0, 5.0, -20.0, -20.0);

   ////////////////
   // boid creation
   ////////////////

   // When creating boids for the predator/prey world, you do so in
   // the following steps:
   //    - Create boid
   //    - Set boid's type (Fly, Sparrow, or Hawk)
   //    - Set boid's perception range (NOTE:  The constructors have defaults, but
   //      the demo shows hunting behavior better with specific values.  Otherwise
   //      you could get Flies that can see farther than the Sparrows--and that
   //      means the Sparrows nearly always starve.
   //    - Set boid's prey (Fly, Sparrow, or Hawk) if any
   //    - Set boid's predator (Fly, Sparrow, or Hawk) if any
   //    - Indicate whether or not this boid can reproduce
   //    - Add the boid to a flock of like boids
   //
   // If so desired, you could also call each boid's SetHunger() method
   // to set their hunger levels here, overriding the random value assigned
   // when the boid was created.  I did that with the Flies and the Hawk to 
   // show how it's done,  The Sparrows I left with the random values instead.

   // build some boids to move around in this world

   myprintf("\nCreating boids\n");

   char name[MaxNameLength];

   for (i = 0; i < MaxFlies; i++) {
	   // create boid
      sprintf(name, "Fly # %d",i);
      Flies[i] = new CBoid(name);
	   // set its type
	   Flies[i]->SetType(Fly);
      // set perception range (overrides defaults)
      Flies[i]->SetPerceptionRange(5.0);
	   // set its prey and predator (if any)
	   Flies[i]->SetPreyOf(None);
	   Flies[i]->SetPredatorOf(Sparrow);
      // set reproduction flag
      Flies[i]->SetReproduction(TRUE);
   }

   for (i = 0; i < MaxSparrows; i++) {
	   // create boid
      sprintf(name, "Sparrow # %d",i);
      Sparrows[i] = new CBoid(name);
	   // set its type
	   Sparrows[i]->SetType(Sparrow);
      // set perception range (overrides defaults)
      Flies[i]->SetPerceptionRange(10.0);
	   // set its prey and predator (if any)
	   Sparrows[i]->SetPreyOf(Fly);
	   Sparrows[i]->SetPredatorOf(Hawk);
      // set reproduction flag
      Flies[i]->SetReproduction(FALSE);
   }

   for (i = 0; i < MaxHawks; i++) {
	   // create boid
      sprintf(name, "Hawk # %d",i);
      Hawks[i] = new CBoid(name);
	   // set its type
	   Hawks[i]->SetType(Hawk);
      // set perception range (overrides defaults)
      Flies[i]->SetPerceptionRange(25.0);
	   // set its prey and predator (if any)
	   Hawks[i]->SetPreyOf(Sparrow);
	   Hawks[i]->SetPredatorOf(None);
      // set reproduction flag
      Flies[i]->SetReproduction(FALSE);
   }

   // build some flocks for the boids to belong to

   for (i = 0; i < MaxFlocks; i++) {
      Flocks[i] = new CFlock();
   }

   myprintf("\nAdding boids to flocks\n");

   // now assign the boids to the flocks

   myprintf("\n  Adding Flies\n");

   for (i = 0; i < MaxFlies; i++) {
      Flocks[0]->AddTo(Flies[i]);
   }

   myprintf("\n  Adding Sparrows\n");

   for (i = 0; i < MaxSparrows; i++) {
      Flocks[1]->AddTo(Sparrows[i]);
   }

   myprintf("\n  Adding Hawks\n");

   for (i = 0; i < MaxHawks; i++) {
      Flocks[2]->AddTo(Hawks[i]);
   }

#ifdef FLOCK_DEBUG
   for (i = 0; i < MaxFlocks; i++) {
      myprintf("Flock %d count = %d\n",i,Flocks[i]->GetCount());
      myprintf("Flock %d live count = %d\n",i,Flocks[i]->GetLiveCount());
      myprintf("Flock %d dead count = %d\n",i,Flocks[i]->GetDeadCount());
   }
   myprintf("Total # of flocks = %d\n",CFlock::FlockCount);
#endif

   myprintf("\nInitializing GL Code\n");

   // initialize the GL code

   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowPosition(200, 0);
   glutInitWindowSize(gWindowWidth, gWindowHeight);
   glutCreateWindow("Flocking With Teeth App");

   glutDisplayFunc(DisplayFunc);
   glutKeyboardFunc(KeyboardFunc);
   glutSpecialFunc(KeyboardSpecialFunc);
   glutReshapeFunc(ReshapeFunc);
   glutIdleFunc(IdleFunc);
  
   Initialize3D();
  
   glutSwapBuffers();
   glutMainLoop();

   // we're done...get outta here

   return (0);
}

// Shutdown.
// Dumps out all the data about the final positions
// of the boids and then shuts down the demo.

void Shutdown (void)
{

   int i;

   // dump data about the boids at the end

   myprintf("\n\n=========\n\n");

   myprintf("Number of generations = %d\n",gFrameNumber);
   
   for (i = 0; i < MaxFlocks; i++) {
      Flocks[i]->PrintData();
   }

   myprintf("\nFinal camera postion = %f %f %f\n", gCameraPos.x,gCameraPos.y,gCameraPos.z);

   // clean up all of our allocation pointers

   delete Box1;

   for (i = 0; i < MaxObstacles; i++) {
      delete Obstacles[i];
   }

   for (i = 0; i < MaxFlocks; i++) {
      delete Flocks[i];
   }

   for (i = 0; i < MaxFlies; i++) {
      delete Flies[i];
   }

   for (i = 0; i < MaxSparrows; i++) {
      delete Sparrows[i];
   }

   for (i = 0; i < MaxHawks; i++) {
      delete Hawks[i];
   }

   // we're done...get outta here

   exit(0);
}

/////////////////
// GLUT Functions
/////////////////

// StringLength()
//
// Returns the string length using font "font".

unsigned int StringLength(void* font, const char* str) 
{

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

void DisplayString(int x, int y, void* font, const char* str) 
{

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

void DrawBox(float ulx, float uly, float lrx, float lry) 
{

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

   char infoString1[] = "Move camera with arrow keys";
   char quitString[]  = "Press <ESC> to Quit";

   int  i,stringSize;

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

   // Draw the worldbox

   Box1->Draw();

   // Draw the obstacles

    for (i = 0; i < MaxObstacles; i++) {
      Obstacles[i]->Draw();
   }

   // Draw the boids

   for (i = 0; i < CFlock::FlockCount; i++) {
      CFlock::ListOfFlocks[i]->Draw();
   }

   /////////////////////
   // Draw some 2D stuff
   /////////////////////

   // Set up an Orthographic Perspective camera

   SetOrthographicCamera();

   // Draw a box for info string 1

   stringSize = StringLength(GLUT_BITMAP_TIMES_ROMAN_24, infoString1);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4ub(0x0f, 0x3f, 0xff, 0x7f);
   DrawBox(0, 0, 20+stringSize, 34);

   // Display info string 1

   glBlendFunc(GL_ONE, GL_ZERO);
   glColor4ub(0xff, 0xff, 0xff, 0x7f);
   DisplayString(10, 10, 
	   GLUT_BITMAP_TIMES_ROMAN_24, infoString1);  

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
      if (gDrawAxes == TRUE) {
         gDrawAxes = FALSE;
      } else {
         gDrawAxes = TRUE;
      }
      break;
   case GLUT_KEY_F10:
      if (gDrawPerceptionDist == TRUE) {
         gDrawPerceptionDist = FALSE;
      } else {
         gDrawPerceptionDist = TRUE;
      }
      break;
   case GLUT_KEY_F11:
      if (gDrawKeepawayDist == TRUE) {
         gDrawKeepawayDist = FALSE;
      } else {
         gDrawKeepawayDist = TRUE;
      }
      break;
   case GLUT_KEY_F12:
      if (gDrawSeparationDist == TRUE) {
         gDrawSeparationDist = FALSE;
      } else {
         gDrawSeparationDist = TRUE;
      }
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

   gCameraPos = vector3(0.0f, 0.0f, Box1->GetBoxLength() * 2.0f);
   gLookatPos = vector3(0.0f, 0.0f, 0.0f);

}

// IdleFunc()
// Idle function for between frames

void IdleFunc(void) 
{

   int i;

   // Call the display routine next time through the main loop  

   glutPostRedisplay();

   // Test:  Time to update the boids?

   if ((timeGetTime() - gLastTime) >= gMilliSecsPerFrame) {

      // yes--update the boids

      for (i = 0; i < CFlock::FlockCount; i++) {
         CFlock::ListOfFlocks[i]->Update();
      }

      // save the current time

      gLastTime = timeGetTime();
   }
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
