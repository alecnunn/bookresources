// Frustum class test driver
// celes@tecgraf.puc-rio.br
// Jul 2003

/* This code is free software; you can redistribute it and/or modify it. 
** The software provided hereunder is on an "as is" basis, and 
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications. 
*/

#include "vgl/frustum.h"
#include "alg/vector.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Initial screen dimension
#define W 600
#define H 400

// Global camera controls
#define DL 0.1f     // linear increment 
#define DA 0.5f     // angular increment
#define DZ 1.0f     // zoom increment

// Display mode
static int help = 1;  // help message on/off

// Global camera parameters
static float fovy = 50.0f;
static float znear = 1.0f;
static float matrix[16] = {1.0f,0.0f,0.0f,0.0f,
	                          0.0f,1.0f,0.0f,0.0f,
																											0.0f,0.0f,1.0f,0.0f,
																											0.0f,0.0f,0.0f,1.0f
                          };
// Global light source position and floor plane corners
static AlgVector light(0.0f,4.0f,0.0f);
static AlgVector corner(-10.0f,0.0f,-10.0f);
static AlgVector edgex(20.0f,0.0f,0.0f);
static AlgVector edgey(0.0f,0.0f,20.0f);


// Draw scene
// The scene is composed by a quad representing the floor and a few
// object on it (a teapot, a sphere, and a scaled cube)
// This function also draws the light frustum used to compute the perspecive 
// shadow map (thouhg no shadow is rendered).
static void drawscene ()
{
	// position light
	float lpos[4];
	lpos[0] = light.x; lpos[1] = light.y; lpos[2] = light.z; lpos[3] = 1.0f;
	glLightfv(GL_LIGHT0,GL_POSITION,lpos);

	// floor
	glColor3f(1.0f,0.6f,0.0f);
	glNormal3f(0.0f,1.0f,0.0f);
	glBegin(GL_QUADS);
		glVertex3f(corner.x,corner.y,corner.z);
		glVertex3f(corner.x+edgex.x,corner.y+edgex.y,corner.z+edgex.z);
		glVertex3f(corner.x+edgex.x+edgey.x,corner.y+edgex.y+edgey.y,corner.z+edgex.z+edgey.z);
		glVertex3f(corner.x+edgey.x,corner.y+edgey.y,corner.z+edgey.z);
	glEnd();

	// teapot
	glColor3f(1.0f,0.0f,0.0f);
	glPushMatrix();
	glTranslatef(0.0f,0.7f,0.0f);
 glutSolidTeapot(1.0);
	glPopMatrix();

	// cube
	glColor3f(0.0f,0.7f,0.0f);
	glPushMatrix();
	glTranslatef(3.0f,0.5f,-5.0f);
	glScalef(2.0f,1.0f,1.0f);
 glutSolidCube(1.0);
	glPopMatrix();

	// sphere
	glColor3f(0.0f,0.0f,1.0f);
	glPushMatrix();
	glTranslatef(-4.0f,0.5f,3.0f);
 glutSolidSphere(0.5,32,32);
	glPopMatrix();

	// light frustum
	glDisable(GL_LIGHTING);
	glColor3f(0.5f,0.5f,0.5f);
	glBegin(GL_LINES);
	 glVertex3f(light.x,light.y,light.z);
		glVertex3f(corner.x,corner.y,corner.z);
	 glVertex3f(light.x,light.y,light.z);
		glVertex3f(corner.x+edgex.x,corner.y+edgex.y,corner.z+edgex.z);
	 glVertex3f(light.x,light.y,light.z);
		glVertex3f(corner.x+edgex.x+edgey.x,corner.y+edgex.y+edgey.y,corner.z+edgex.z+edgey.z);
	 glVertex3f(light.x,light.y,light.z);
		glVertex3f(corner.x+edgey.x,corner.y+edgey.y,corner.z+edgey.z);
	glEnd();
	glEnable(GL_LIGHTING);
}

// Display message on screen
static void putstring (char* s, float x, float y)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glColor3f(1.0f,1.0f,1.0f);
	glRasterPos3d(x,y,0.8);
	for (int i=0; s[i]; i++)
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,s[i]);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// Extract and display viewing info
// This code tests the VglFrustum class extracting camera information.
static void extractviewinfo ()
{
	char msg[128];
	VglFrustum f;     /* create object with current matrices */
	AlgVector eye = f.GetEyePos();
	AlgVector dir = f.GetViewDir(); dir.Normalize();
	AlgVector up = f.GetViewUp(); up.Normalize();
	float znear = f.GetNearDist();
	float zfar = f.GetFarDist();
	float fovx = f.GetFovx();
	float fovy = f.GetFovy();

	sprintf(msg,"** Extracted information **");
	putstring(msg,-0.95,0.92);
	sprintf(msg,"Eye position: %.2f %.2f %.2f",eye.x,eye.y,eye.z);
	putstring(msg,-0.95,0.82);
	sprintf(msg,"View direction: %.2f %.2f %.2f",dir.x,dir.y,dir.z);
	putstring(msg,-0.95,0.74);
	sprintf(msg,"Up direction: %.2f %.2f %.2f",up.x,up.y,up.z);
	putstring(msg,-0.95,0.66);
	sprintf(msg,"Near and far distances: %.2f %.2f",znear,zfar);
	putstring(msg,-0.95,0.58);
	sprintf(msg,"X and Y angles of view: %.2f %.2f",fovx,fovy);
	putstring(msg,-0.95,0.50);
}

// The function below tests the AlgFrustum class extracting 
// light information based on the perspective shadow matrix
// proposed by Heckbert & Herf (see Real-Time Rendering
// book of Akenine-Moller & Haines, page 255)
// It exemplifies how we could use the matrix to extract light information

// Enumerate light frustum planes
enum {
	LIGHT_LEFT = 0,
	LIGHT_RIGHT,
	LIGHT_BOTTOM,
	LIGHT_TOP,
	LIGHT_NEAR,
	LIGHT_FAR
};

// Extract and display light info
static void extractlightinfo ()
{
	// build projective matrix
	AlgVector edgew(corner.x-light.x,corner.y-light.y,corner.z-light.z);
	AlgVector nu = Cross(edgew,edgey); 
	AlgVector nv = Cross(edgex,edgew); 
	AlgVector nw = Cross(edgey,edgex);

	float qu = 1/nu.Dot(edgex);
	float qv = 1/nv.Dot(edgey);
	float qw = 1/nw.Dot(edgew);

	float m[16];
	m[0] = qu*nu.x; m[4] = qu*nu.y; m[8]  = qu*nu.z; m[12] = -qu*nu.Dot(corner);
	m[1] = qv*nv.x; m[5] = qv*nv.y; m[9]  = qv*nv.z; m[13] = -qv*nv.Dot(corner); 
	m[2] = 0;       m[6] = 0;       m[10] = 0;       m[14] = 1; 
	m[3] = qw*nw.x; m[7] = qw*nw.y; m[11] = qw*nw.z; m[15] = -qw*nw.Dot(light);

	// create frustum class and set appropriate canonical planes
	AlgFrustum f(6);
	f.SetCanonicalPlane(LIGHT_LEFT,1,0,0,0);
	f.SetCanonicalPlane(LIGHT_RIGHT,1,0,0,-1);
	f.SetCanonicalPlane(LIGHT_BOTTOM,0,1,0,0);
	f.SetCanonicalPlane(LIGHT_TOP,0,1,0,-1);
	f.SetCanonicalPlane(LIGHT_NEAR,0,0,1,-1);
	f.SetCanonicalPlane(LIGHT_FAR,0,0,0,1);
	f.SetVertexMatrix(m);

	// extract information
	AlgPlane left = f.GetPlane(LIGHT_LEFT);
	AlgPlane right = f.GetPlane(LIGHT_RIGHT);
	AlgPlane top = f.GetPlane(LIGHT_TOP);
	AlgPlane pnear = f.GetPlane(LIGHT_NEAR);
	AlgVector apex = Intersect(left,right,top); 
	AlgVector dir = pnear.GetNormal(); dir.Normalize(); 

	// display info
	char msg[128];
	sprintf(msg,"Light position: %.2f %.2f %.2f",apex.x,apex.y,apex.z);
	putstring(msg,-0.95,0.38);
	sprintf(msg,"Receiver orientation: %.2f %.2f %.2f",dir.x,dir.y,dir.z);
	putstring(msg,-0.95,0.30);
}

// Display help message
static void showhelp ()
{
	char msg[128];
	sprintf(msg,"** Help: use keys to change viewer and light source **");
	putstring(msg,-0.95,-0.39);
	sprintf(msg,"w,s,a,d,r,f: move viewer");
	putstring(msg,-0.95,-0.47);
	sprintf(msg,"arrow keys: turn viewer");
	putstring(msg,-0.95,-0.55);
	sprintf(msg,"q,e: roll viewer");
	putstring(msg,-0.95,-0.63);
	sprintf(msg,"+,-: zoom in/out");
	putstring(msg,-0.95,-0.71);
	sprintf(msg,"W,S,A,D,R,F: move light source");
	putstring(msg,-0.95,-0.79);
	sprintf(msg,"h: help message on/off");
	putstring(msg,-0.95,-0.87);
	sprintf(msg,"ESC: quit the program");
	putstring(msg,-0.95,-0.95);
}

// Reshape callback
static void reshape (int w, int h)
{
 float white[4] = {1.0f,1.0f,1.0f,1.0f};
 glViewport(0,0,w,h);
 glClearColor(0,0,0,1); 
 glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
 glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
 glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
 glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,50.0);
 glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);
}

// Display callback
static void display ()
{
 int vp[4]; glGetIntegerv(GL_VIEWPORT,vp);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy,(float)vp[2]/vp[3],znear,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(matrix);
	gluLookAt(0,2,10,0,0,0,0,1,0);

	// draw scene
	drawscene();

	// display help message
	if (help)
 	showhelp();

	// extract information
	extractviewinfo();
	extractlightinfo();

	glutSwapBuffers();
}

// Keyboard callback
static void keyboard (unsigned char key, int , int )
{
	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;
	float rz = 0.0f;
	float lx = 0.0f;
	float ly = 0.0f;
	float lz = 0.0f;

	switch (key)
	{
		case 's': dz -= DL; break;
		case 'w': dz += DL; break;
		case 'd': dx -= DL; break;
		case 'a': dx += DL; break;
		case 'r': dy -= DL; break;
		case 'f': dy += DL; break;
		case 'e': rz -= DA; break;
		case 'q': rz += DA; break;
		case '+': fovy -= DZ; break;
		case '-': fovy += DZ; break;
		case 'S': lz += DL; break;
		case 'W': lz -= DL; break;
		case 'D': lx += DL; break;
		case 'A': lx -= DL; break;
		case 'R': ly += DL; break;
		case 'F': ly -= DL; break;
		case 'h': case 'H': help ^= 1; break;
		case 27: exit(0); break;
	}
	// accumulate transformation using OpenGL matrix operations
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(dx,dy,dz);
	glRotatef(rz,0.0f,0.0f,1.0f);
	glMultMatrixf(matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
	glPopMatrix();

	// update light position
	light.x += lx;
	light.y += ly;
	light.z += lz;

 display();
}

// Special keyboard callback
static void special (int key, int x, int y)
{
	float rx = 0.0f;
	float ry = 0.0f;
	switch (key)
	{
		case GLUT_KEY_UP:    rx += DA; break;
		case GLUT_KEY_DOWN:  rx -= DA; break;
		case GLUT_KEY_RIGHT: ry += DA; break;
		case GLUT_KEY_LEFT:  ry -= DA; break;
		default: return;
	}
	// accumulate transformation using OpenGL matrix operations
	glPushMatrix();
	glLoadIdentity();
	glRotatef(rx,1.0f,0.0f,0.0f);
	glRotatef(ry,0.0f,1.0f,0.0f);
	glMultMatrixf(matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
	glPopMatrix();

 display();
}


// Main function
int main (int argc, char* argv[])
{
 // open GLUT 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
 glutInitWindowSize(W,H); 

 // create window
 glutCreateWindow ("VglFrustum");
 glutReshapeFunc(reshape); 
 glutDisplayFunc(display); 
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

 // interact... 
 glutMainLoop();
 return 0;
}
