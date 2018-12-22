#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glaux.lib")

#include <stdio.h>


#include "cFrustum.h"

#define RADtoDEG(a) ( (a)*57.29577951308232286465f )

cFrustum frustum;   // The frustum


// variable that define and store some spheres and points
	#define MAX_SPHERES 200
	#define MAX_POINTS  1000
	#define MAX_AABB    300
	#define MAX_OBB     200

	struct cSphere{
		cVector3f Center;
		float     Radius;

		float ColorRed, ColorGreen, ColorBlue;
	};

	struct cPoint{
		cVector3f Position;

		float ColorRed, ColorGreen, ColorBlue;
	};

	struct cAABB{
		cVector3f Center;
		cVector3f HalfDimensions;

		float ColorRed, ColorGreen, ColorBlue;
	};

	// An oriented Bounding Box (builded as an AABB that has a rotation)
	struct cOBB{
		cVector3f Center;
		cVector3f HalfDimensions;
		cQuaternionf Rotation;

		float ColorRed, ColorGreen, ColorBlue;
	};

	cSphere aSpheres[ MAX_SPHERES ];
	cPoint  aPoints[ MAX_POINTS ];
	cAABB   aAABB[ MAX_AABB ];
	cOBB    aOBB[ MAX_OBB ];



// Camera Global orientation

	float AnglePitch(0);
	float AngleYaw(0);



// A math helper function (not very optimized but sweet)
// It generate a random float number that is between a and b
#define RANDOM(a,b) ( ((b)-(a)) * (((float)(rand()%1000)+1)/1000) + (a) ) 


// Configuration
bool bDrawSolidFrustum(true);
int CameraType(0); // 0: Out of the frustum
				   // 1: A bit back from the frustum	
				   // 2: In the frustum

void myinit(void)
{
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

	// Initialize some variables. 
	// This is done only once: at the aplicatio startup
	frustum.SetPerspective(60, 1, 2,100);


	// Generate random objects

	for ( int i=0; i<MAX_SPHERES; ++i )
	{
		aSpheres[i].Radius     = RANDOM(0.5f,3);
		aSpheres[i].Center.x   = RANDOM(-50,50);
		aSpheres[i].Center.y   = RANDOM(-50,50);
		aSpheres[i].Center.z   = RANDOM(-100,0);
		aSpheres[i].ColorRed   = RANDOM(0,1);
		aSpheres[i].ColorGreen = RANDOM(0,1);
		aSpheres[i].ColorBlue   = RANDOM(0,1);
	}

	for ( i=0; i<MAX_POINTS; ++i )
	{
		aPoints[i].Position.x = RANDOM(-50,50);
		aPoints[i].Position.y = RANDOM(-50,50);
		aPoints[i].Position.z = RANDOM(-100,0);
		aPoints[i].ColorRed   = RANDOM(0,1);
		aPoints[i].ColorGreen = RANDOM(0,1);
		aPoints[i].ColorBlue   = RANDOM(0,1);
	}

	for ( i=0; i<MAX_AABB; ++i )
	{
		aAABB[i].Center.x = RANDOM(-50,50);
		aAABB[i].Center.y = RANDOM(-50,50);
		aAABB[i].Center.z = RANDOM(-100,0);
		aAABB[i].HalfDimensions.x = RANDOM(1,3);
		aAABB[i].HalfDimensions.y = RANDOM(1,3);
		aAABB[i].HalfDimensions.z = RANDOM(1,3);
		aAABB[i].ColorRed   = RANDOM(0,1);
		aAABB[i].ColorGreen = RANDOM(0,1);
		aAABB[i].ColorBlue   = RANDOM(0,1);
	}

	for ( i=0; i<MAX_OBB; ++i )
	{
		aOBB[i].Center.x = RANDOM(-50,50);
		aOBB[i].Center.y = RANDOM(-50,50);
		aOBB[i].Center.z = RANDOM(-100,0);
		aOBB[i].HalfDimensions.x = RANDOM(1,3);
		aOBB[i].HalfDimensions.y = RANDOM(1,3);
		aOBB[i].HalfDimensions.z = RANDOM(1,3);
		aOBB[i].Rotation.SetFromEulerRAD(RANDOM(-3.14f,3.14f), RANDOM(0,6.28f), RANDOM(0,6.28f) );
		aOBB[i].ColorRed   = RANDOM(0,1);
		aOBB[i].ColorGreen = RANDOM(0,1);
		aOBB[i].ColorBlue   = RANDOM(0,1);
	}

}

void __stdcall myReshape(GLsizei w, GLsizei h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(60, 1, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

//--------------------------------------------------------
//  KEY FUNCTIONS
//--------------------------------------------------------

void __stdcall Key_Pressed_Left ()
{
	AngleYaw += 1;
	if (AngleYaw >360) AngleYaw -= 360;
}

//--------------------------------------------------------

void __stdcall Key_Pressed_Right ()
{
	AngleYaw -= 1;
	if (AngleYaw<0) AngleYaw += 360;
}

//--------------------------------------------------------

void __stdcall Key_Pressed_Up ()
{
	AnglePitch -= 1;

	if (AnglePitch <-60) AnglePitch = -60;
}

//--------------------------------------------------------

void __stdcall Key_Pressed_Down ()
{
	AnglePitch += 1;

	if (AnglePitch > 60) AnglePitch = 60;
}

//--------------------------------------------------------

void __stdcall Key_Pressed_Toogle_Solid_Frustum ()
{
	bDrawSolidFrustum = !bDrawSolidFrustum;
}

//--------------------------------------------------------

void __stdcall Key_Pressed_Toogle_Camera ()
{
	CameraType++;
	if (CameraType >2) CameraType = 0;
}

//--------------------------------------------------------


void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	if (CameraType != 0)
	{
		if (CameraType == 1) glTranslatef(0,0,-5);

		glRotatef(AnglePitch,1,0,0);
		glRotatef(-AngleYaw,0,1,0);
		glTranslatef(0,0,50);
	}

	// Render the spheres that are visible from the frustum
		for (int i=0; i<MAX_SPHERES; ++i)
			if ( frustum.IsSphereIn(aSpheres[i].Center,aSpheres[i].Radius) )
			{
			   glColor3f (aSpheres[i].ColorRed, aSpheres[i].ColorGreen, aSpheres[i].ColorBlue );
			   glTranslatef(aSpheres[i].Center.x, aSpheres[i].Center.y, aSpheres[i].Center.z);
			   auxWireSphere( aSpheres[i].Radius );
			   glTranslatef(-aSpheres[i].Center.x, -aSpheres[i].Center.y, -aSpheres[i].Center.z);
			}

	// Render the points that are visible from the frustum
		glBegin(GL_POINTS);
		for (i=0; i<MAX_POINTS; ++i)
			if ( frustum.IsPointIn(aPoints[i].Position) )
			{
			   glColor3f (aPoints[i].ColorRed, aPoints[i].ColorGreen, aPoints[i].ColorBlue );
			   glVertex3f(aPoints[i].Position.x, aPoints[i].Position.y, aPoints[i].Position.z);
			}
		glEnd();

	// Render the Axis Align Boxes that are visible from the frustum
		for (i=0; i<MAX_AABB; ++i)
			if ( frustum.IsAABBIn(aAABB[i].Center-aAABB[i].HalfDimensions, aAABB[i].Center+aAABB[i].HalfDimensions) )
			{
			   glColor3f (aAABB[i].ColorRed, aAABB[i].ColorGreen, aAABB[i].ColorBlue );
			   glTranslatef(aAABB[i].Center.x, aAABB[i].Center.y, aAABB[i].Center.z);
			   auxWireBox( aAABB[i].HalfDimensions.x, aAABB[i].HalfDimensions.y, aAABB[i].HalfDimensions.z );
			   glTranslatef(-aAABB[i].Center.x, -aAABB[i].Center.y, -aAABB[i].Center.z);
			}

	// Render the Oriented Boxes that are visible from the frustum
		for (i=0; i<MAX_OBB; ++i)
			if ( frustum.IsOBBIn(aOBB[i].Center,aOBB[i].HalfDimensions, aOBB[i].Rotation) )
			{
			   glColor3f (aOBB[i].ColorRed, aOBB[i].ColorGreen, aOBB[i].ColorBlue );
			   glPushMatrix();
				   glTranslatef(aOBB[i].Center.x, aOBB[i].Center.y, aOBB[i].Center.z);

				   float axisAngle, axisX, axisY, axisZ;
				   aOBB[i].Rotation.GetAngleAxisRAD(axisAngle, axisX, axisY, axisZ );
				   glRotatef( RADtoDEG(-axisAngle), axisX, axisY, axisZ );

				   auxWireBox( aOBB[i].HalfDimensions.x, aOBB[i].HalfDimensions.y, aOBB[i].HalfDimensions.z );
			   glPopMatrix();
			}

	// draw a white frustum if its not the camera number '2'

	if (CameraType != 2)
		if (bDrawSolidFrustum) frustum.DrawSolid();
						  else frustum.DrawOutline();

    auxSwapBuffers();
}

void __stdcall loop()
{
	// Here the loop has to update the camera orientation but in this implementation
	// for simplicity there is no real "camera". 
	// However a virtual camera is constructed and the matrix is extrated to CameraMatrix
		glLoadIdentity();
		glRotatef(180,0,1,0); // To make it start pointing to the negative z axis
							  // Because the load identity cause to make it look to the possitive z axis
		glTranslatef(0,0,50);
		glRotatef(AngleYaw,0,1,0);
		glRotatef(AnglePitch,1,0,0);

		float CameraMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, CameraMatrix);

	// The camera is built and the matrix is stored. The next is to construct the frustum

		cVector3f* Right    = (cVector3f*) &CameraMatrix[0];
		cVector3f* Up	    = (cVector3f*) &CameraMatrix[4];
		cVector3f* Forward	= (cVector3f*) &CameraMatrix[8];
		cVector3f* Position = (cVector3f*) &CameraMatrix[12];
		
		frustum.Build(*Position,*Forward,*Right,*Up);

	// and that's all: the frustum is updated


	RenderScene();
}



int main(int argc, char** argv)
{
	printf("Example of Frustum Culling using the Radar Approach\n\n");
	printf("  Objects Tested:\n");
	printf("    - Points\n");
	printf("    - Spheres\n");
	printf("    - Axis Aligned Bounding Boxes (AABB)\n");
	printf("    - Oriented Bounding Boxes (OBB)\n\n\n");
	
	printf("  Configuration:\n\n");
	printf("    Cursors:  Rotate Camera\n");
	printf("    C      :  Change Camera\n");
	printf("    X      :  Toogle drawing the frustum 'planes'\n");
	printf("    Esc    :  Exit\n");
    auxInitDisplayMode (AUX_DOUBLE | AUX_RGBA | AUX_DEPTH);
    auxInitPosition (175, 50, 350, 350);
    auxInitWindow (argv[0]);

    myinit();
 

	auxKeyFunc(AUX_A, Key_Pressed_Left);
	auxKeyFunc(AUX_a, Key_Pressed_Left);
	auxKeyFunc(AUX_LEFT, Key_Pressed_Left);

	auxKeyFunc(AUX_D, Key_Pressed_Right);
	auxKeyFunc(AUX_d, Key_Pressed_Right);
	auxKeyFunc(AUX_RIGHT, Key_Pressed_Right);

	auxKeyFunc(AUX_W, Key_Pressed_Up);
	auxKeyFunc(AUX_w, Key_Pressed_Up);
	auxKeyFunc(AUX_UP, Key_Pressed_Up);

	auxKeyFunc(AUX_S, Key_Pressed_Down);
	auxKeyFunc(AUX_s, Key_Pressed_Down);
	auxKeyFunc(AUX_DOWN, Key_Pressed_Down);

	auxKeyFunc(AUX_X, Key_Pressed_Toogle_Solid_Frustum);
	auxKeyFunc(AUX_x, Key_Pressed_Toogle_Solid_Frustum);
	
	auxKeyFunc(AUX_C, Key_Pressed_Toogle_Camera);
	auxKeyFunc(AUX_c, Key_Pressed_Toogle_Camera);
	
	auxReshapeFunc(myReshape);

    auxMainLoop(loop);
	return 0;
}