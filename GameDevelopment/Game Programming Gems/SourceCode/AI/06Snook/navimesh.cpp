/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "mtxlib.h"
#include "actor.h"
#include "navigationmesh.h"


////
// Globals
//
int 		gFrameNumber = 0;
vector3 	gCameraPos;
vector3 	gLookatPos;
const float 		gKeyStep = 10.0f;
const unsigned int	gWindowWidth  = 640;
const unsigned int	gWindowHeight = 480;
bool gShowNaviMesh = false;
bool gAutomatedMode = false;

Actor PathObject;		// red cone that does the pathfinding
Actor ControlObject;	// blue cone controlled by the player
Actor PlayPen;			// the playpen host environment
NavigationMesh NaviMesh;// our navigation mesh
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

//
// external geometry
//
extern unsigned long cone_totalpoints;
extern vector3 cone_points[7];
extern unsigned long cone_totalpolys;
extern unsigned short cone_polys[10][3];

extern unsigned long map_totalpoints;
extern vector3 map_points[77];
extern unsigned long map_totalpolys;
extern unsigned short map_polys[75][3];

extern unsigned long playpen_totalpoints;
extern vector3 playpen_points[381];
extern unsigned long playpen_totalpolys;
extern unsigned short playpen_polys[456][3];

////
// Functions
//


// main()
//
#ifdef _WIN32

int WINAPI WinMain(
  HINSTANCE hInstance,      // handle to current instance
  HINSTANCE hPrevInstance,  // handle to previous instance
  LPSTR lpCmdLine,          // command line
  int nCmdShow              // show state
)
{

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
#else

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

#endif
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
  char infoString[] = "Path Demo App";
  char quitString[] = "<ESC> to Quit";
  char inputString[] = "Arrow Keys Move.";
  char pathString[] = "<SPACE> to find path";
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

  // Draw our objects
  if(gShowNaviMesh)
  {
	  NaviMesh.Render();
  }
  PathObject.Render();
  ControlObject.Render();
  PlayPen.Render();

  // Set up an Orthographic Perspective camera
  SetOrthographicCamera();

  //
  // Draw some 2D stuff
  //

  // Draw a box for the input string
  stringSize = StringLength(GLUT_BITMAP_TIMES_ROMAN_24, pathString);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4ub(0xff, 0x3f, 0x0f, 0x7f);
  DrawBox(0, gWindowHeight, 20+stringSize, gWindowHeight-34);

  // Display the inputString
  glBlendFunc(GL_ONE, GL_ZERO);
  glColor4ub(0xff, 0xff, 0xff, 0x7f);
  DisplayString(10, gWindowHeight-24, 
		GLUT_BITMAP_TIMES_ROMAN_24, pathString);

  // Draw a box for the pathString
  stringSize = StringLength(GLUT_BITMAP_TIMES_ROMAN_24, inputString);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4ub(0x0f, 0x3f, 0xff, 0x7f);
  DrawBox(gWindowWidth-20-stringSize, gWindowHeight, gWindowWidth, gWindowHeight-34);

  // Display the pathString
  glBlendFunc(GL_ONE, GL_ZERO);
  glColor4ub(0xff, 0xff, 0xff, 0x7f);
  DisplayString(gWindowWidth-10-stringSize, gWindowHeight-24, 
		GLUT_BITMAP_TIMES_ROMAN_24, inputString);

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
  case 'w': // Up
  case 'W':
    gCameraPos.y += gKeyStep;
    break;
  case 's': // Down
  case 'S':
    gCameraPos.y -= gKeyStep;
    break;
  case 'a': // Left
  case 'A':
    gCameraPos.x -= gKeyStep;
    break;
  case 'd': // Right
  case 'D':
    gCameraPos.x += gKeyStep;
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
  case 'T': // Toggle NavigationMesh visibility
	  gShowNaviMesh = !gShowNaviMesh;
    break;
  case 'r': //  Toggle Automated random mode
  case 'R': 
	  gAutomatedMode = !gAutomatedMode;
    break;
	case ' ': //  go to ControlObject location
		if (!gAutomatedMode) PathObject.GotoLocation(ControlObject.Position(), ControlObject.CurrentCell());
	break;
	case '1': //  go to ControlObject location
		PathObject.SetMaxSpeed(1.0f);
		ControlObject.SetMaxSpeed(1.0f);
	break;
	case '2': //  go to ControlObject location
		PathObject.SetMaxSpeed(2.0f);
		ControlObject.SetMaxSpeed(2.0f);
	break;
	case '3': //  go to ControlObject location
		PathObject.SetMaxSpeed(3.0f);
		ControlObject.SetMaxSpeed(3.0f);
	break;
	case '4': //  go to ControlObject location
		PathObject.SetMaxSpeed(4.0f);
		ControlObject.SetMaxSpeed(4.0f);
	break;
	case '5': //  go to ControlObject location
		PathObject.SetMaxSpeed(5.0f);
		ControlObject.SetMaxSpeed(5.0f);
	break;
	case '6': //  go to ControlObject location
		PathObject.SetMaxSpeed(6.0f);
		ControlObject.SetMaxSpeed(6.0f);
	break;
	case '7': //  go to ControlObject location
		PathObject.SetMaxSpeed(7.0f);
		ControlObject.SetMaxSpeed(7.0f);
	break;
	case '8': //  go to ControlObject location
		PathObject.SetMaxSpeed(8.0f);
		ControlObject.SetMaxSpeed(8.0f);
	break;
	case '9': //  go to ControlObject location
		PathObject.SetMaxSpeed(9.0f);
		ControlObject.SetMaxSpeed(9.0f);
	break;
  }
}


// KeyboardSpecialFunc()
//
// This function handles the arrow keys.
//
void KeyboardSpecialFunc(int key, int x, int y) 
{
	if (!gAutomatedMode)
	{
		switch (key) 
		{
			case GLUT_KEY_UP:
				ControlObject.SetMovementZ(2.0f);
				break;
			case GLUT_KEY_DOWN:
				ControlObject.SetMovementZ(-2.0f);
				break;
			case GLUT_KEY_LEFT:
				ControlObject.SetMovementX(2.0f);
				break;
			case GLUT_KEY_RIGHT:
				ControlObject.SetMovementX(-2.0f);
				break;
		}
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
  matrix44 perspMatrix = FrustumMatrix44(-1.6f, 1.6f, -1.6f, 1.6f, 4, 512);
  glLoadMatrixf((float*)&perspMatrix);
#endif
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

#if 0
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
  glOrtho(0, 640, 0, 480, -1, 1);
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

  // Call the routine to update our objects
  PathObject.Update();
  ControlObject.Update();
  PlayPen.Update();

	if (gAutomatedMode)
	{
		int forceX = (rand() % 20)-10;
		int forceZ = (rand() % 20)-10;

		if (ControlObject.Movement().x < 5.0f)
		{
			int forceX = (rand() % 20)-10;
			int forceZ = (rand() % 20)-10;

			ControlObject.AddMovement(vector3(forceX, 0, forceZ));
		}
		if (!PathObject.PathIsActive())
		{
			PathObject.GotoRandomLocation();
		}
	}
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

	int i;

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
  gCameraPos = vector3(10.0f, 180.0f, -230.0f);
  gLookatPos = vector3(0, 0, 0);

  //
  // Create a test navigation mesh
  //
    NaviMesh.Clear();

	for (i=0;i<map_totalpolys;++i)
	{
		const vector3& vertA = map_points[map_polys[i][0]];
		const vector3& vertB = map_points[map_polys[i][1]];
		const vector3& vertC = map_points[map_polys[i][2]];

		// some art programs can create linear polygons which have two or more
		// identical vertices. This creates a poly with no surface area,
		// which will wreak havok on our navigation mesh algorythms.
		// We only except polygons with unique vertices.
		if ((vertA != vertB) && (vertB != vertC) && (vertC != vertA))
		{
			NaviMesh.AddCell(vertA, vertB, vertC);
		}
	}

  NaviMesh.LinkCells();

  //
  // Create a PathObject on the NavigationMesh
  //
  PathObject.Create(&NaviMesh, vector3(0.0f, 0.0f, 0.0f),0);

  Actor::Triangle Poly;
  memset(&Poly.color[0][0], 0x00, 4*3);

	for (i=0;i<cone_totalpolys;++i)
	{
		  Poly.vert[0]=cone_points[cone_polys[i][0]];
		  Poly.vert[1]=cone_points[cone_polys[i][1]];
		  Poly.vert[2]=cone_points[cone_polys[i][2]];
		
		  Poly.color[0][0] = 0xff;
		  Poly.color[1][0] = 0xff;
		  Poly.color[2][0] = 0xff;

		PathObject.AddTriangle(Poly);
	}

  //
  // Create a ControlObject on the NavigationMesh
  //
  ControlObject.Create(&NaviMesh, vector3(0.0f, 0.0f, 0.0f),0);

  memset(&Poly.color[0][0], 0x00, 4*3);

	for (i=0;i<cone_totalpolys;++i)
	{
		  Poly.vert[0]=cone_points[cone_polys[i][0]];
		  Poly.vert[1]=cone_points[cone_polys[i][1]];
		  Poly.vert[2]=cone_points[cone_polys[i][2]];
		
		  Poly.color[0][2] = 0xff;
		  Poly.color[1][2] = 0xff;
		  Poly.color[2][2] = 0xff;

		ControlObject.AddTriangle(Poly);
	}

  //
  // Create our PlayPen. It does not use the NavigationMesh
  //
  PlayPen.Create(0, vector3(0.0f, 0.0f, 0.0f),0);

	memset(&Poly.color[0][0], 0x00, 4*3);
  Poly.color[0][3] = 0xff;
  Poly.color[1][3] = 0xff;
  Poly.color[2][3] = 0xff;
	
	for (i=0;i<playpen_totalpolys;++i)
	{
		  Poly.vert[0]=playpen_points[playpen_polys[i][0]];
		  Poly.vert[1]=playpen_points[playpen_polys[i][1]];
		  Poly.vert[2]=playpen_points[playpen_polys[i][2]];
		
		Poly.color[0][0] = Poly.color[0][1] = Poly.color[0][2] = 64+(char(Poly.vert[0].y)*2);
		Poly.color[1][0] = Poly.color[1][1] = Poly.color[1][2] = 64+(char(Poly.vert[1].y)*2);
		Poly.color[2][0] = Poly.color[2][1] = Poly.color[2][2] = 64+(char(Poly.vert[2].y)*2);

		PlayPen.AddTriangle(Poly);
	}

}

