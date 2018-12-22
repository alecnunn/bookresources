/* Copyright (C) Stan Melax, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Stan Melax, 2000"
 */
//
//       Games Programming Gems
//
//  Demo program that includes RotationArc
//  which is used by the virtual trackball function
//  to spin the logo via left mouse drag.
//
#ifdef _WIN32
#include "windows.h"
#endif
#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <assert.h>
#include "mtxlib.h"



// extrememly reduced quaternion library:
class Quaternion
{
 public:
	float x,y,z,w;
	Quaternion(){x=y=z=0.0f;w=1.0f;};
	//Quaternion(vector3 axis,float angle); axis-angle
	//Quaternion(float _x,float _y,float _z,float _w){x=_x;y=_y;z=_z;w=_w;};
};

Quaternion operator*(Quaternion a,Quaternion b) 
{
	Quaternion c;
	c.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z; 
	c.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y; 
	c.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x; 
	c.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w; 
	return c;
}

//
//        RotationArc()
//
//  This is the function from Games Programming Gems
//  that we are using in the VirtualTrackBall function
//
Quaternion RotationArc(vector3 v0,vector3 v1)
{
  Quaternion q;
  // v0.normalize(); 
  // v1.normalize();  // If vector is already unit length then why do it again?
  vector3 c = CrossProduct(v0,v1);
  float   d = DotProduct(v0,v1);
  float   s = (float)sqrt((1+d)*2);
  q.x = c.x / s;
  q.y = c.y / s;
  q.z = c.z / s;
  q.w = s /2.0f;
  return q;
}

vector3 planelineintersection(vector3 n,float d,vector3 p1,vector3 p2)
{
  // returns the point where the line p1-p2 intersects the plane n&d
  vector3 dif  = p2-p1;
  float dn= DotProduct(n,dif);
  float t = -(d+DotProduct(n,p1) )/dn;
  return p1 + (dif*t);
}

void MakeMatrix4x4(float *m,const vector3 &v,const Quaternion &q) 
{
	// a lean function for filling m[16] with
	// a 4x4 transformation matrix based on 
	// translation v and rotation q
	// This routine would likely be used by an opengl
	// programmer calling glmultmatrix()
	m[0] = 1-2*(q.y*q.y+q.z*q.z);
	m[1] = 2*(q.x*q.y+q.w*q.z)  ;
	m[2] = 2*(q.x*q.z-q.w*q.y)  ;
	m[3] = 0            ;
	m[4] = 2*(q.x*q.y-q.w*q.z)  ;	
	m[5] = 1-2*(q.x*q.x+q.z*q.z);
	m[6] = 2*(q.y*q.z+q.w*q.x)  ;
	m[7] = 0    ;   
	m[8] = 2*(q.x*q.z+q.w*q.y)  ; 
	m[9] = 2*(q.y*q.z-q.w*q.x)  ; 
	m[10]= 1-2*(q.x*q.x+q.y*q.y);
	m[11]= 0    ; 
	m[12] = v.x ;
	m[13] = v.y ;
	m[14] = v.z ;
	m[15] = 1.0f;
}


////
// Globals
//
// note: I just use camera at 0,0,0 and 0 orientation (z in face)
//
vector3 	position;    // of the object
Quaternion  orientation; // of the object!!
unsigned int	gWindowWidth  = 640;
unsigned int	gWindowHeight = 480;
float viewangle = 45.0f;
int mousex=0;
int mousey=0;
int mousexold=0;
int mouseyold=0;




vector3 MouseDir(int x,int y){
  // Converts x and y in screen coords releative to opengl window
  // To a vector pointing into the world
  assert(viewangle >0.0f);     // Note this must be Perspective
  GLint vp[4];
  // just use the current viewport!
  glGetIntegerv(GL_VIEWPORT,vp);
  x-=vp[0];
  y-=vp[1];
  // cull x,y with respect to the viewport
  float  spread = (float)tan(viewangle/2*3.14/180);
  return vector3(spread * (x-vp[2]/2.0f)  /(vp[3]/2.0f),
                 spread * (y-vp[3]/2.0f) /(vp[3]/2.0f),
                 -1.0f);     
}


Quaternion VirtualTrackBall(vector3 cop,vector3 cor,vector3 dir1,vector3 dir2) 
{
	// Implement track ball functionality to spin stuf on the screen
	//  cop   center of projection
	//  cor   center of rotation
	//  dir1  old mouse direction 
	//  dir2  new mouse direction
	// pretend there is a sphere around cor.  Then find the points
	// where dir1 and dir2 intersect that sphere.  Find the
	// rotation that takes the first point to the second.
	// If the line "dir" doesn't hit the sphere then take the closest
	// point on the sphere to that line.
	float m;
	// compute plane 
	vector3 nrml = cor - cop;
	float fudgefactor = 1.0f/(nrml.length() * 0.25f); // since trackball proportional to distance from cop
	nrml.normalize();
	float dist = -DotProduct(nrml,cor);
	vector3 u= planelineintersection(nrml,dist,cop,cop+dir1);
	u=u-cor;
	u=u*fudgefactor;
	m= u.length();
	if(m>1) 
    {
        u=u*1.0f/m;
    }
	else 
    {
		u=u - (nrml * (float)sqrt(1-m*m));
	}
	vector3 v= planelineintersection(nrml,dist,cop,cop+dir2);
	v=v-cor;
	v=v*fudgefactor;
	m= v.length();
	if(m>1) 
    {
        v=v*1.0f/m;
    }
	else 
    {
		v=v - (nrml * (float)sqrt(1-m*m));
	}
	return RotationArc(u,v);
}




////
// Functions
//
int tid=0; // texture id
void inittexture() {
	extern unsigned long gpgtexture[128*128];
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&tid);
	assert(tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, gpgtexture);
}

void drawlogo(){
  float m[16];
  glPushMatrix();
  MakeMatrix4x4(m,position,orientation);
  glMultMatrixf(m);

  // Draw the spinner

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, tid);

  glBegin(GL_QUADS);

  glColor4ub(0xff, 0, 0, 0xff);
  glTexCoord2f(0.0F, 0.0F);
  glVertex3f(-50, -50, 0);

  glColor4ub(0xff, 0, 0xff, 0xff);
  glTexCoord2f(1.0F, 0.0F);
  glVertex3f(-50,  50, 0);

  glColor4ub(0, 0xff, 0, 0xff);
  glTexCoord2f(1.0F, 1.0F);
  glVertex3f( 50,  50, 0);

  glColor4ub(0, 0, 0xff, 0xff);
  glTexCoord2f(0.0F, 1.0F);
  glVertex3f( 50, -50, 0);

  glEnd();

  glDisable(GL_TEXTURE_2D);

  // Pop the rotation matrix from the MODELVIEW stack
  glPopMatrix();
}


void mousepassivemotion(int x,int y){
	mousexold=mousex;
	mouseyold=mousey;
	mousex=x;
	mousey = gWindowHeight-y;
}
void mouseactivemotion(int x,int y){
	mousepassivemotion(x,y); // update mouse x&y
	vector3 mdir_old=MouseDir(mousexold,mouseyold);
	vector3 mdir_new=MouseDir(mousex,mousey);
	Quaternion q = VirtualTrackBall(vector3(0,0,0),position,mdir_old,mdir_new);
	orientation = q*orientation;
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
  //matrix44 perspMatrix = FrustumMatrix44(-1.6, 1.6, -1.6, 1.6, 4, 512);
  //glLoadMatrixf((float*)&perspMatrix);
  glLoadIdentity();
  gluPerspective(viewangle, (float)gWindowWidth/(float)gWindowHeight,0.5f,500.0f);

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



// DisplayFunc()
//
void DisplayFunc(void) {
  char infoString[] = "GPG Mouse Spin Demo";
  char quitString[] = "<ESC> to Quit";
  int  stringSize;

  // Clear the frame buffer and Z buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // Set up a Perspective Projection camera
  SetPerspectiveCamera();

  // Set up current camera
  gluLookAt(0,0,0,
	    position.x, position.y, position.z, 
	    0, 1, 0);  // Up vector

  // Draw some 3D 
  glBlendFunc(GL_ONE, GL_ZERO);

  // Draw the spinner
  drawlogo();
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
  }
}


// KeyboardSpecialFunc()
//
// This function handles the arrow keys.
//
void KeyboardSpecialFunc(int key, int x, int y) {
}



// IdleFunc()
//
void IdleFunc(void) {
  // Call the display routine next time through the main loop  
  glutPostRedisplay();
}


// ReshapeFunc()
//
// This function is called when the user changes the window size.
//
void ReshapeFunc(int w, int h) {
  // Modify the viewport for the new window settings
  glViewport (0, 0, w, h);
  gWindowWidth  = w;
  gWindowHeight = h;
}


// main()
//
#ifdef _WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nShowCmd) {
#else
int main(int argc, char* argv[]) {
#endif

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowPosition(200, 0);
  glutInitWindowSize(gWindowWidth, gWindowHeight);
  glutCreateWindow("GPG Demo VirtualTrackBall");
  
  glutMotionFunc(mouseactivemotion);
  glutPassiveMotionFunc(mousepassivemotion);
  glutDisplayFunc(DisplayFunc);
  glutKeyboardFunc(KeyboardFunc);
  glutSpecialFunc(KeyboardSpecialFunc);
  glutReshapeFunc(ReshapeFunc);
  glutIdleFunc(IdleFunc);
  
  // InitializeProgram
  // Enable Gouraud shading
  glShadeModel(GL_SMOOTH);
  glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
  glEnable(GL_BLEND);
  glFrontFace(GL_CCW);
  glPolygonMode(GL_FRONT, GL_FILL);

  // Initialize camera and object position
  position   = vector3(0, 0, -200);
  inittexture();
  glutSwapBuffers();

  glutMainLoop();
  
  return 0;
}




