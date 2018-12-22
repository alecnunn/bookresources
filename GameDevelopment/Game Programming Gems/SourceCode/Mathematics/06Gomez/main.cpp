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

#include "body.h"
#include "mathphys.h"


// TEMP HACK:  x is right, y is forward, z is up
//
// 3D Camera
//
struct Camera : public COORD_FRAME
{
	float D, F;	//near and far plane
	VECTOR3 V;	//velocity
	VECTOR3 W;	//angular velocity

	Camera()
		:	D(0.001f),	//HACK:  very near D helps prevent shadow problems
			F(100)
	{}

	Camera( float d, float f, float a )
		: D(d), F(f)
	{}
};


//
// 3 vertex indices
//
struct Triangle
{
	unsigned short V[3];//vertex indices
	//NOTE:  No more than 64K vertices

	//default
	Triangle()
	{
		//more efficient to not initialize
	}

	//initialize
	Triangle( unsigned short v0, unsigned short v1, unsigned short v2 )
	{
		V[0] = v0;
		V[1] = v1;
		V[2] = v2;
	}
};


//
// Triangles, Vertices, Normals, Topology
//
//NOTE:  No more than 64K triangles or vertices
struct BoatGeometry
{
	Triangle *F;//faces
	unsigned short FCount;//number of faces, face normals

	VECTOR3 *V;//vertices
	VECTOR3 *N;//vertex normals
	float *dA;
	unsigned short VCount;

	BoatGeometry()
		:	F(NULL), FCount(0),
			V(NULL), N(NULL), dA(NULL), VCount(0)
	{}

	~BoatGeometry()
	{
		if( F )		delete F;
		if( V )		delete V;
		if( N )		delete N;
		if( dA )	delete dA;
	}
};


//
// 2D Array of floats
//
struct FloatArray2D
{
	float* pData;//pointer to externally allocated data
	const long Width;//array width

	FloatArray2D( float* pd, const long w )
		: pData(pd), Width(w)
	{}

	//indexing operator
	//read-only
	const float* operator [] ( const long i ) const		{ return pData + i*Width; }

	//write-to
	float* operator [] ( const long i )					{ return pData + i*Width; }
};




//globals
Camera gCamera;
RIGID_BODY gBoat;
BoatGeometry gBoatGeometry;



//water stuff
const long N = 128 + 1;//number of gridpoints along a side
const float c = 2;//wave space, meters/sec
const float h = 2*float(0.1*c*sqrt(2));//grid cell width (min for stability, assuming dt<=0.1)
const float L = h*(N-1);//width of entire grid
float z[N][N];//z[n] values
float z1[N][N];//z[n-1] and z[n+1] values
float d[N][N];//damping coefficients
FloatArray2D Z( (float*)z, N );//z[n] values
FloatArray2D Z1( (float*)z1, N );//z[n-1] and z[n+1] values
VECTOR3 V[N][N];//triangle vertices
VECTOR3 VN[N][N];//vertex normals
GLint Tri[N-1][N][2];//triangle strip elements











/////////////////////////////////////////////////////////////////////////////////////////
void animateWater
(
	FloatArray2D& z,
	FloatArray2D& z1,
	const float dt
)
{
	//Integrate the solution of the 2D wave equation.

	//OPTIMIZATION:  This code has been written for
	//clarity, not speed!  It would be better to use
	//pointer arithmetic instead of indexing.

	//precalculate coefficients
	const float A = (c*dt/h)*(c*dt/h);
	const float B = 2 - 4*A;
	long i, j;

		//edges are unchanged
		for( i=1 ; i<N-1 ; i++ )
		{
			for( j=1 ; j<N-1 ; j++ )
			{
				//integrate, replacing z[n-1] with z[n+1] in place
				z1[i][j] =	A*( z[i-1][j] + z[i+1][j] + z[i][j-1] + z[i][j+1] )
							+ B*z[i][j] - z1[i][j];

				//apply damping coefficients
				z1[i][j] *= d[i][j];

			}
		}

		//swap pointers
		Swap( z.pData, z1.pData );
}


/////////////////////////////////////////////////////////////////////////////////////////
void interpolateWater( const float x, const float y, float& zw, VECTOR3& nw )
{
	const float rh = 1 / h;
	//fractional index
	float u = (x + 0.5*L) * rh;
	float v = (y + 0.5*L) * rh;
	//lower-left vertex of the enclosing grid cell
	const long i = long( u );
	const long j = long( v );
	//interpolation coefficients
	const float a = u - i;
	const float b = v - j;
	const float ab = a*b;

	//if the position is outside of the grid, give a fake value
	if( i<0 || N<=i || j<0 || N<=j )
	{
		zw = 0;
		nw.x = nw.y = 0;
		nw.z = 1;
	}
	else
	{
		//bilinearly interpolate zw and nw
		zw = (1-a-b+ab) * z[i][j] + (b-ab) * z[i][j+1] + (a-ab) * z[i+1][j] + ab * z[i+1][j+1];
		nw = (1-a-b+ab) * VN[i][j] + (b-ab) * VN[i][j+1] + (a-ab) * VN[i+1][j] + ab * VN[i+1][j+1];
		nw.normalize();//if the interpolation was spherical, this wouldn't be necessary

		/*
		//DEBUG:  interpolated height should be between z_min and z_max
		float z_min = Min( Min(z[i][j],z[i][j+1]), Min(z[i+1][j],z[i+1][j+1]) );
		float z_max = Max( Max(z[i][j],z[i][j+1]), Max(z[i+1][j],z[i+1][j+1]) );

		if( !(z_min<=zw && zw<=z_max) )
		{
			int kd=0;
		}
		//*/
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
void animateBoat( const float dt )
{
	const float k = 1000 * 9.8;//water density * gravity
	VECTOR3 F_net(0,0,0);//net force
	VECTOR3 T_net(0,0,0);//net torque

		//gravity
		F_net.z -= 9.8 * gBoat.M;

		//buoyancy
		for( long i=0 ; i<gBoatGeometry.VCount ; i++ )
		{
			//vector from CM to hull vertex, in world space
			const VECTOR3 r = gBoat.transformVectorToParent( gBoatGeometry.V[i] );
			const VECTOR3 p = gBoat.position() + r;//hull vertex
			VECTOR3 nw;//water normal
			float zw;//water height

			//bilinearly interpolate the height
			//and normal of the water surface
			//at the (x,y) position
			interpolateWater( p.x, p.y, zw, nw );

			//only calculate buoyancy
			//for submerged portins
			if( zw > p.z )
			{
				//velocity of hull vertex with respect to water, assuming water is still
				const VECTOR3 v = gBoat.V + gBoat.W.cross(r);
				//drag force is proportional to relative velocity
				const VECTOR3 Fd = - 2500 * v;//HACK:  magic number
				//buoyant force is proportional to weight of water displaced
				const VECTOR3 Fb = - k * (zw - p.z) * gBoatGeometry.dA[i] * gBoatGeometry.N[i].z * nw;
				//total force exerted on this hull vertex
				const VECTOR3 F = Fd + Fb;

					F_net += F;//add to force
					T_net += r.cross(F);//add to torque
			}
		}

	const VECTOR3 dv = (1 / gBoat.M) * F_net * dt;
	const VECTOR3 dw = gBoat.dwdt( T_net ) * dt;

		gBoat.translate( gBoat.V * dt );
		gBoat.rotate( gBoat.W * dt );
		gBoat.V += dv;
		gBoat.W += dw;
		gBoat.W *= 0.999f;//HACK: fake energy loss prevents instability
}


/////////////////////////////////////////////////////////////////////////////////////
void idle()
{
	const float dt = 0.025f;//HACK:  constant time step

	//animate the camera
	gCamera.translate( gCamera.V * dt );

	//animate the water
	animateWater( Z, Z1, dt );

	//animate the boat
	animateBoat( dt );

	//redraw
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////////
void drawScene()
{
	long i, j;

	//draw the water

	//update the vertices and the normals
	for( i=0 ; i<N; i++ )
	{
		for( j=0 ; j<N; j++ )
		{
			//only the z needs to be updated
			V[i][j].z = z[i][j];

			//only the x and y need to be updated
			VN[i][j].x = (z[i][j-1] - z[i][j+1]);
			VN[i][j].y = (z[i-1][j] - z[i+1][j]);
			//OPTIMIZATION:  For clarity, the
			//normals are updated here, but it
			//might be faster to update them
			//inside the integration loop since
			//the neighboring points have already
			//been loaded into the cache.
		}
	}

	//draw triangles
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, V );
	glNormalPointer( GL_FLOAT, 0, VN );

	//allow the driver to normalize the normals.
	//If the video card has T&L, it might be
	//faster than doing it in the CPU.
	glEnable( GL_NORMALIZE );

	glColor3f( 0, 0, 1 );//blue

	glPolygonMode( GL_FRONT, GL_LINE );
	glPolygonMode( GL_BACK, GL_LINE );
	for( i=0 ; i<N-1; i++ )
	{
		glBegin( GL_TRIANGLE_STRIP );
		{
			for( j=0 ; j<N; j++ )
			{
				glArrayElement( Tri[i][j][0] );
				glArrayElement( Tri[i][j][1] );
			}
		}
		glEnd();
	}

	glDisable( GL_NORMALIZE );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	//draw the boat
	glColor3f( 1, 1, 1 );//white
	glPolygonMode( GL_FRONT, GL_FILL );//solid
	glPolygonMode( GL_BACK, GL_FILL );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, gBoatGeometry.V );
	glNormalPointer( GL_FLOAT, 0, gBoatGeometry.N );

	glPushMatrix();
	{
		const VECTOR3& R0 = gBoat.R[0];
		const VECTOR3& R1 = gBoat.R[1];
		const VECTOR3& R2 = gBoat.R[2];
		const VECTOR3& O = gBoat.O;
		float M[4][4] =
		{
			{ R0.x, R0.y, R0.z, 0 },
			{ R1.x, R1.y, R1.z, 0 },
			{ R2.x, R2.y, R2.z, 0 },
			{ O.x, O.y, O.z, 1 },
		};

		glMultMatrixf( (float*)M );
		glDrawElements( GL_TRIANGLES, 3*gBoatGeometry.FCount, GL_UNSIGNED_SHORT, gBoatGeometry.F );
	}
	glPopMatrix();

	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
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

	float light_position[]	= { 10,10,10,1 };

	glLightfv( GL_LIGHT0, GL_POSITION, light_position );

	//clear frame buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	drawScene();

	//flush and page flip
	glutSwapBuffers();
}


/////////////////////////////////////////////////////////////////////////////////////////
void initializeWater()
{
	long i,j;

	//triangle vertices, normals, z-values
	for( i=0 ; i<N ; i++ )
	{
		for( j=0 ; j<N ; j++ )
		{
			VECTOR3& v = V[i][j];
			VECTOR3& n = VN[i][j];

			//vertices, center of
			//grid is at x=0, y=0
			v.x = i*h - L/2;
			v.y = j*h - L/2;
			v.z = 0;

			//normal
			n.z = 2*h;
			//later we will let the openGL
			//driver normalize them for us

			//z-values
			z[i][j] = 0;
			z1[i][j] = 0;

			//damping coefficients
			d[i][j] = 0.9999f;

			//create a circular island in
			//the center of the pool
			const float xc = v.x - 1;
			const float yc = v.y - 1;
			if( (xc*xc + yc*yc) <= 10 )
			{
				d[i][j] = 0;
			}
		}
	}

	//normals at the edge of the
	//grid point straight up
	for( i=0 ; i<N ; i++ )
	{
		VN[i][0].z = 1;
		VN[i][N-1].z = 1;
	}

	for( j=0 ; j<N ; j++ )
	{
		VN[0][j].z = 1;
		VN[N-1][j].z = 1;
	}

	//triangle strip elements
	for( i=0 ; i<N-1; i++ )
	{
		for( j=0 ; j<N; j++ )
		{
			Tri[i][j][0] = i*N + j;
			Tri[i][j][1] = (i+1)*N + j;
		}
	}

	//create a big splash
	z[N/3][N/3] = z1[N/3][N/3] = 10;
}


/////////////////////////////////////////////////////////////////////////////////////
void initializeBoat()
{
	//Physical Properties
	gBoat = RIGID_BODY(	15000, VECTOR3(6,4,2) );
	gBoat.position( -5, -5, 0 );

	//Geometry
	//allocate arrays
	gBoatGeometry.FCount = 12;
	gBoatGeometry.F = new Triangle[gBoatGeometry.FCount];//faces

	gBoatGeometry.VCount = 8;
	gBoatGeometry.V = new VECTOR3[gBoatGeometry.VCount];//vertices
	gBoatGeometry.N = new VECTOR3[gBoatGeometry.VCount];//vertex normals
	gBoatGeometry.dA = new float[gBoatGeometry.VCount];//area patches

	//triangles
	gBoatGeometry.F[0] = Triangle(0,1,2);
	gBoatGeometry.F[1] = Triangle(0,2,3);
	gBoatGeometry.F[2] = Triangle(4,5,6);
	gBoatGeometry.F[3] = Triangle(4,6,7);
	gBoatGeometry.F[4] = Triangle(4,7,1);
	gBoatGeometry.F[5] = Triangle(4,1,0);
	gBoatGeometry.F[6] = Triangle(7,6,2);
	gBoatGeometry.F[7] = Triangle(7,2,1);
	gBoatGeometry.F[8] = Triangle(6,5,3);
	gBoatGeometry.F[9] = Triangle(6,3,2);
	gBoatGeometry.F[10] = Triangle(5,4,0);
	gBoatGeometry.F[11] = Triangle(5,0,3);

	//vertices
	gBoatGeometry.V[0] = VECTOR3( 3, 2, 1);
	gBoatGeometry.V[1] = VECTOR3(-3, 2, 1);
	gBoatGeometry.V[2] = VECTOR3(-3,-2, 1);
	gBoatGeometry.V[3] = VECTOR3( 3,-2, 1);
	gBoatGeometry.V[4] = VECTOR3( 3, 2,-1);
	gBoatGeometry.V[5] = VECTOR3( 3,-2,-1);
	gBoatGeometry.V[6] = VECTOR3(-3,-2,-1);
	gBoatGeometry.V[7] = VECTOR3(-3, 2,-1);

	//vertex normals
	//(pointing outward through
	//the vertices)
	gBoatGeometry.N[0] = gBoatGeometry.V[0].unit();
	gBoatGeometry.N[1] = gBoatGeometry.V[1].unit();
	gBoatGeometry.N[2] = gBoatGeometry.V[2].unit();
	gBoatGeometry.N[3] = gBoatGeometry.V[3].unit();
	gBoatGeometry.N[4] = gBoatGeometry.V[4].unit();
	gBoatGeometry.N[5] = gBoatGeometry.V[5].unit();
	gBoatGeometry.N[6] = gBoatGeometry.V[6].unit();
	gBoatGeometry.N[7] = gBoatGeometry.V[7].unit();

	//area patches (1/8th of
	//total surface area)
	gBoatGeometry.dA[0] =
	gBoatGeometry.dA[1] =
	gBoatGeometry.dA[2] =
	gBoatGeometry.dA[3] =
	gBoatGeometry.dA[4] =
	gBoatGeometry.dA[5] =
	gBoatGeometry.dA[6] =
	gBoatGeometry.dA[7] = 2*(6*4 + 6*2 + 4*2) / 8;
}


/////////////////////////////////////////////////////////////////////////////////////
void initScene()
{
	//init lights
	float black[]			= { 0,0,0,0 };
	float white[]			= { 1,1,1,1 };
	float amb_light[]		= { 0.1f,0.1f,0.1f,1 };

	glShadeModel( GL_SMOOTH );
	//global ambient light
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );

	glMaterialfv( GL_FRONT, GL_AMBIENT, white );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, white );

	//update material color with glColor()
	glColorMaterial( GL_FRONT, GL_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );

	//light0 properties
	glLightfv( GL_LIGHT0, GL_AMBIENT, amb_light );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, white );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glDisable( GL_CULL_FACE );

	//camera
	gCamera.position( 13, -13, 13 );
	gCamera.R[0] = VECTOR3( 0.31f, 0.95f, 0 );
	gCamera.R[1] = VECTOR3( -0.80f, 0.26f, -0.54f);
	gCamera.R[2] = VECTOR3( -0.51f, 0.16f, 0.84f );

	//water grid
	initializeWater();

	//boat
	initializeBoat();
}


/////////////////////////////////////////////////////////////////////////////////////
void keyboard( unsigned char key, int x, int y )
{
	switch( key )
	{
	case 27:
		exit(0);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////////////
void special( int key, int x, int y )
{
	switch( key )
	{
	case GLUT_KEY_UP:
		gCamera.translate( 0.1f * gCamera.R[1] );//forward
		break;

	case GLUT_KEY_DOWN:
		gCamera.translate( -0.1f * gCamera.R[1] );//backward
		break;

	case GLUT_KEY_RIGHT:
		gCamera.translate( 0.1f * gCamera.R[0] );//strafe right
		break;

	case GLUT_KEY_LEFT:
		gCamera.translate( -0.1f * gCamera.R[0] );//strafe left
		break;
	}
}


int gx0, gy0;//for mouse movement

/////////////////////////////////////////////////////////////////////////////////////
void mouseMove( int x1, int y1 )
{
	const int dx = x1 - gx0;
	const int dy = y1 - gy0;

	//turn the camera left and right as the mouse moves
	gCamera.rotateAboutX( -0.01f*dy );//look up/down
	gCamera.rotate( -0.01f*VECTOR3(0,0,1)*dx );//turn right/left

	//remember for next time
	gx0 = x1;
	gy0 = y1;
}


/////////////////////////////////////////////////////////////////////////////////////
void mouseClick( int button, int state, int x1, int y1 )
{
	//catch button state
	switch( state )
	{
	case GLUT_DOWN:

		//reset so the rotation is continuous
		gx0 = x1;
		gy0 = y1;

		switch( button )
		{
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		}
		break;
	case GLUT_UP:
		switch( button )
		{
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		}
		break;
	}
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


/////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
	//initialize display mode
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL );
	glutInitWindowSize( 800, 600 );
	glutInitWindowPosition( 100, 100 );
	glutCreateWindow( "Floating Boat" );

	//setup callbacks
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( special );
	glutMouseFunc( mouseClick );
	glutMotionFunc( mouseMove );
	glutReshapeFunc( reshape );
	glutVisibilityFunc( visible );

	//initialize the scene and objects
	initScene();

	glutMainLoop();
	return 0;
}

//EOF
