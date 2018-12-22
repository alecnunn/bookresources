/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <GL/glut.h>
#ifndef _WIN32
#include <sys/timeb.h>
#endif

#include "body.h"
#include "mathphys.h"


// NOTE:  x is right, y is forward, z is up
//
// 3D Camera
//
struct Camera : public COORD_FRAME
{
	float D, F;	//near and far plane
	VECTOR3 V;	//velocity
	VECTOR3 W;	//angular velocity


	Camera()
		: D(0.1f), F(100)
	{}

	Camera( float d, float f, float a )
		: D(d), F(f)
	{}
};


Camera gCamera;
RIGID_BODY	gSpinningBox( 1, VECTOR3(1,2,3) );






/////////////////////////////////////////////////////////////////////////////////////
void drawScene()
{
	//draw the ground plane
	glBegin( GL_QUADS );
	{
		glColor3f( 1, 1, 1 );//white
		glNormal3f( 0, 0, 1 );
		glVertex3f( 10, 10, 0 );

		glColor3f( 1, 1, 1 );//white
		glNormal3f( 0, 0, 1 );
		glVertex3f( -10, 10, 0 );

		glColor3f( 1, 1, 1 );//white
		glNormal3f( 0, 0, 1 );
		glVertex3f( -10, -10, 0 );

		glColor3f( 1, 1, 1 );//white
		glNormal3f( 0, 0, 1 );
		glVertex3f( 10, -10, 0 );
	}
	glEnd();

	/*
	//draw a sphere
	glPushMatrix();
	{
		glColor3f( 0, 0, 1 );//blue
		glTranslatef( 0, 0, 1 );
		glutSolidSphere( 1, 64, 64 );
//		glutWireSphere( 1, 64, 64 );
	}
	glPopMatrix();
	//*/

	//draw a box spinning in the air
	glColor3f( 1, 1, 1 );//white
	glPushMatrix();
	{
		//HACK:  Add in scale factor
		const VECTOR3& R0 = gSpinningBox.Dim.x * gSpinningBox.R[0];
		const VECTOR3& R1 = gSpinningBox.Dim.y * gSpinningBox.R[1];
		const VECTOR3& R2 = gSpinningBox.Dim.z * gSpinningBox.R[2];
		const VECTOR3& O = gSpinningBox.O;
		float M[4][4] =
		{
			{ R0.x, R0.y, R0.z, 0 },
			{ R1.x, R1.y, R1.z, 0 },
			{ R2.x, R2.y, R2.z, 0 },
			{ O.x, O.y, O.z, 1 },
		};

		glMultMatrixf( (float*)M );
		glutSolidCube( 1 );
	}
	glPopMatrix();
}


/////////////////////////////////////////////////////////////////////////////////////
void display()
{
	//load camera
	glMatrixMode( GL_MODELVIEW );
	const VECTOR3 R0 = gCamera.R[0];
	const VECTOR3 R1 = gCamera.R[2];//swap and negate to match
	const VECTOR3 R2 = -gCamera.R[1];//OGL's camera convention
	const VECTOR3& O = gCamera.O;//position
	float M[4][4] =
	{
		{ R0.x, R1.x, R2.x, 0 },//X
		{ R0.y, R1.y, R2.y, 0 },//Y
		{ R0.z, R1.z, R2.z, 0 },//Z
		{ -R0.dot(O), -R1.dot(O), -R2.dot(O), 1 }//translation
	};
	glLoadMatrixf( (float*)M );
	//NOTE:  light position must be set
	//AFTER the camera is loaded but
	//BEFORE the models are rendered
	//because it is multiplied by the
	//modelview matrix and stored
	float light_position[]	= { 10,10,10,1 };
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );

	//clear frame buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	drawScene();
	//*/

	//flush and page flip
	glutSwapBuffers();
}


/////////////////////////////////////////////////////////////////////////////////////
void idle()
{
#ifdef _WIN32
	static float t0 = (float)clock() / CLOCKS_PER_SEC;
	float t1 = (float)clock() / CLOCKS_PER_SEC;
#else
	// This doesn't seem to work properly under Linux!
	static struct timeb storig;
	static int firstTime = 1;
	static float t0; 
	struct timeb st1;
	float t1;
	
	// Set up initial time
	if (firstTime == 1) {
		ftime(&storig);
		firstTime = 0;
		t0 = 0.0f;
	}

	// Get current time
	ftime(&st1);
	t1 = (float)((st1.time - storig.time) + (float)st1.millitm/1000.0f);
#endif
	float dt = t1 - t0;
	printf("dt=%f\n", dt);

	t0 = t1;//for the next frame

	//HACK:  prevent large time steps
	if( dt>0.1f )
		dt = 0.1f;

	//animate the camera
	gCamera.translate( gCamera.V * dt );

	//rotate the box
	const VECTOR3 dw = gSpinningBox.dwdt( VECTOR3(0,0,0) ) * dt;
	gSpinningBox.rotate( gSpinningBox.W * dt );
	gSpinningBox.W += dw;
	gSpinningBox.W *= 0.999f;//HACK: fake energy loss

	//redraw
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////////
void initScene()
{
	//init lights
	float black[]			= { 0,0,0,0 };
	float white[]			= { 1,1,1,1 };
	float mat_shininess[]	= { 50 };
	float amb_light[]		= { 0.1f,0.1f,0.1f,1 };

	glShadeModel( GL_SMOOTH );
	//global ambient light
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );

	glMaterialfv( GL_FRONT, GL_AMBIENT, white );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white );
	glMaterialfv( GL_FRONT, GL_SPECULAR, white );
	glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

	//update material color with glColor()
	glColorMaterial( GL_FRONT, GL_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );

	//light0 properties
	glLightfv( GL_LIGHT0, GL_AMBIENT, black );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, white );
	glLightfv( GL_LIGHT0, GL_SPECULAR, white );
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION,	0.005f );
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION,		0.001f );
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION,	1.0 );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	//init camera (this looked good)
	gCamera.position( 5.8f, -5.2f, 4.5f );
	gCamera.R[0] = VECTOR3( 0.75f, 0.66f, 0 );
	gCamera.R[1] = VECTOR3( -0.58f, 0.66f, -0.48f);
	gCamera.R[2] = VECTOR3( -0.32f, 0.36f, 0.88f );

	//init spinning box
	gSpinningBox.position( 0, 0, 2.5f );
	gSpinningBox.W = VECTOR3( 0.5f, 0.5f, 10.0f );
}


/////////////////////////////////////////////////////////////////////////////////////
void keyboard( unsigned char key, int x, int y )
{
	int kd=0;

	//BUG:  arrow keys don't trigger this callback.

	switch( key )
	{
	case 'e':
	case 'E':
		gCamera.rotateAboutX( -0.05f );//look down
		break;

	case 'd':
	case 'D':
		gCamera.rotateAboutX( 0.05f );//look up
		break;

	case 's':
	case 'S':
		gCamera.rotate( 0.05f*VECTOR3(0,0,1) );//turn left
		break;

	case 'f':
	case 'F':
		gCamera.rotate( -0.05f*VECTOR3(0,0,1) );//turn right
		break;

	case 27:
		exit(0);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////////////
void mouse( int button, int state, int x1, int y1 )
{
	//catch button state
	switch( state )
	{
	case GLUT_DOWN:
		switch( button )
		{
		case GLUT_LEFT_BUTTON:
			gCamera.V = 2*gCamera.R[1];//forward at 2 m/s
			break;
		case GLUT_RIGHT_BUTTON:
			gCamera.V = -2*gCamera.R[1];//backward at 2 m/s
			break;
		}
		break;
	case GLUT_UP:
		switch( button )
		{
		case GLUT_LEFT_BUTTON:
			gCamera.V = VECTOR3(0,0,0);//stop
			break;
		case GLUT_RIGHT_BUTTON:
			gCamera.V = VECTOR3(0,0,0);//stop
			break;
		}
		break;
	}

	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////////
void visible( int vis )
{
	if( vis == GLUT_VISIBLE )
	{
		glutIdleFunc( idle );
	}
	else
	{
		glutIdleFunc( NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////////////
void reshape( int w, int h )
{
	const float D = gCamera.D;//near plane
	const float F = gCamera.F;//far plane
	const float Q = F / (F-D);
	const float a = float(h) / w;//aspect ratio

	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -D, D, -a*D, a*D, D, F );
}


/////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
	//initialize display mode
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL );
	glutInitWindowSize( 800, 600 );
	glutInitWindowPosition( 100, 100 );
	glutCreateWindow( "Rigid Body Demo" );

	//setup callbacks
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutReshapeFunc( reshape );
	glutVisibilityFunc( visible );

	//initialize the scene and objects
	initScene();

	glutMainLoop();
	return 0;
}

//EOF
