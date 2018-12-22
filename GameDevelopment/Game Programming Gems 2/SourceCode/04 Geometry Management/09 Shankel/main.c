/* Copyright (C) Jason Shankel, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jason Shankel, 2001"
 */
/*
Game Gems II Skybox Demo
by
Jason Shankel
*/

#include <GL/glut.h>
#include <GL/glaux.h>
#include "GL/glext.h"
#include <stdio.h>
#include <stdlib.h>

/*
Camera controls
*/
static GLfloat cameraTheta=0,cameraPhi=0;
static GLint mouseX=0,mouseY=0,mouseDownX=0,mouseDownY=0;

/*
Skybox rendering mode
*/
#define kSkyboxMode2d 0
#define kSkyboxModeCubeEnvironment 1
static int skyboxMode = kSkyboxMode2d; 
static int cubeMapAvailable = 0;
static int hudMode = 1;

/*
Texture objects
*/
static GLuint textureObjectCubeMap;
static GLuint textureObjects2d[6];

/*
Shiny object parameters
*/
static int rotateObject = 1;
static GLint objectAngle=0;
#define kTorus 0
#define kSphere 1
#define kTeapot 2
#define kCube 3
#define kNoObject 4
#define kObjectTypeCount 5
static int objectType = kTorus;


/*
Rendering functions
*/
static void drawSkybox2d(void);
static void drawSkyboxCubeEnvironment(void);
static void hudEnd(void);
static void hudBegin(void);
static void hudDisplayString(int x,int y,void *font,const char *str);

/*
Init/Shutdown 
*/
static GLboolean initSkybox(void);
static void shutdownSkybox(void);

/*
Image loading
SWAP_BYTES is used to flip inverted *.bmp files
*/
GLboolean loadImage(GLenum target,const char *fileName);
#define SWAP_BYTES(b1,b2) (b1)=(b1)^(b2);(b2)=(b1)^(b2);(b1)=(b1)^(b2);


/*
GLUT callbacks
*/
static void onTimer(int value);
static void onDisplay(void);
static void onMouseMoveButtonDown(int x,int y);
static void onMouseDown(int b,int s,int x,int y);
static void onSpecialKey(int key,int x,int y);
static void onKey(unsigned char key,int x,int y);


/*
Draw skybox using cube environment mapping.  The skybox cube is drawn as two triangle fans, centered around opposite corners of the cube
*/
void 
drawSkyboxCubeEnvironment(void)
{
	/*
	Setup cube environment mapping
	*/
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB,textureObjectCubeMap);
	
	/*
	Disable the depth buffer when rendering the skybox
	*/
	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);
	
	/*
	Draw a fan around 10,10,10
	*/
	glBegin(GL_TRIANGLE_FAN);
	
	glTexCoord3f(1,1,1);
	glVertex3f(10,10,10);
	
	glTexCoord3f(1,1,-1);
	glVertex3f(10,10,-10);
	
	glTexCoord3f(-1,1,-1);
	glVertex3f(-10,10,-10);
	
	glTexCoord3f(-1,1,1);
	glVertex3f(-10,10,10);
	
	glTexCoord3f(-1,-1,1);
	glVertex3f(-10,-10,10);
	
	glTexCoord3f(1,-1,1);
	glVertex3f(10,-10,10);

	glTexCoord3f(1,-1,-1);
	glVertex3f(10,-10,-10);
	
	glTexCoord3f(1,1,-1);
	glVertex3f(10,10,-10);
	
	glEnd();

	/*
	Draw a fan around -10,-10,-10
	*/
	glBegin(GL_TRIANGLE_FAN);

	glTexCoord3f(-1,-1,-1);
	glVertex3f(-10,-10,-10);

	glTexCoord3f(-1,-1,1);
	glVertex3f(-10,-10,10);

	glTexCoord3f(1,-1,1);
	glVertex3f(10,-10,10);

	glTexCoord3f(1,-1,-1);
	glVertex3f(10,-10,-10);
	
	glTexCoord3f(1,1,-1);
	glVertex3f(10,10,-10);
	
	glTexCoord3f(-1,1,-1);
	glVertex3f(-10,10,-10);
	
	glTexCoord3f(-1,1,1);
	glVertex3f(-10,10,10);
	
	glTexCoord3f(-1,-1,1);
	glVertex3f(-10,-10,10);
	
	glEnd();
	
	/*
	Reactivate depth buffer
	*/
	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);

	/*
	Now draw shiny object
	*/

	/*
	Set up reflection mapping coordinate generation
	*/
	glTexGenf(GL_S,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP_ARB);
	glTexGenf(GL_T,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP_ARB);
	glTexGenf(GL_R,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP_ARB);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	/*
	Set up object matrix
	*/
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,0,-8);
	glRotatef(cameraPhi,1,0,0);
	glRotatef(cameraTheta,0,1,0);
	
	/*
	Rotating about two axes with different angles gives a nice tumbling effect
	*/
	glRotatef((GLfloat)objectAngle,1,0,0);
	glRotatef((GLfloat)2*objectAngle,0,1,0);

	/*
	Rotate texture matrix to match camera matrix
	*/
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-cameraTheta,0,1,0);
	glRotatef(-cameraPhi,1,0,0);

	/*
	Some glut objects require matrix-mode to be set to GL_MODELVIEW
	*/
	glMatrixMode(GL_MODELVIEW);

	/*
	Draw Object
	*/
	if(objectType != kNoObject)
	{
		switch(objectType)
		{
			case kTorus:
				glutSolidTorus(1,2,16,16);
				break;

			case kSphere:
				glutSolidSphere(3,16,16);
				break;

			case kTeapot:
				glutSolidTeapot(3);
				break;

			case kCube:
				glutSolidCube(3);
				break;
		};
	}

	/*
	Restore texture and modelview matrices
	*/
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	/*
	Disable reflection mapping
	*/
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	
}

/*
Draw skybox using six faces and six 2d textures
*/
void
drawSkybox2d(void)
{
	/*
	Enable 2d texture mapping
	*/
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glEnable(GL_TEXTURE_2D);
	
	/*
	Disable the depth buffer when rendering the skybox
	*/
	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);


	/*
	Rear Face
	*/
	glBindTexture(GL_TEXTURE_2D,textureObjects2d[0]);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1,0);
	glVertex3f(10,10,10);
	glTexCoord2f(1,1);
	glVertex3f(10,-10,10);
	glTexCoord2f(0,1);
	glVertex3f(-10,-10,10);
	glTexCoord2f(0,0);
	glVertex3f(-10,10,10);
	glEnd();
	
	/*
	Front Face
	*/
	glBindTexture(GL_TEXTURE_2D,textureObjects2d[1]);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1,0);
	glVertex3f(-10,10,-10);
	glTexCoord2f(1,1);
	glVertex3f(-10,-10,-10);
	glTexCoord2f(0,1);
	glVertex3f(10,-10,-10);
	glTexCoord2f(0,0);
	glVertex3f(10,10,-10);
	glEnd();

	/*
	Right Face
	*/
	glBindTexture(GL_TEXTURE_2D,textureObjects2d[2]);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1,0);
	glVertex3f(10,10,-10);
	glTexCoord2f(1,1);
	glVertex3f(10,-10,-10);
	glTexCoord2f(0,1);
	glVertex3f(10,-10,10);
	glTexCoord2f(0,0);
	glVertex3f(10,10,10);
	glEnd();

	/*
	Left Face
	*/
	glBindTexture(GL_TEXTURE_2D,textureObjects2d[3]);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1,0);
	glVertex3f(-10,10,10);
	glTexCoord2f(1,1);
	glVertex3f(-10,-10,10);
	glTexCoord2f(0,1);
	glVertex3f(-10,-10,-10);
	glTexCoord2f(0,0);
	glVertex3f(-10,10,-10);
	glEnd();
	
	/*
	Top Face
	*/
	glBindTexture(GL_TEXTURE_2D,textureObjects2d[4]);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0,1);
	glVertex3f(-10,10,10);
	glTexCoord2f(0,0);
	glVertex3f(-10,10,-10);
	glTexCoord2f(1,0);
	glVertex3f(10,10,-10);
	glTexCoord2f(1,1);
	glVertex3f(10,10,10);
	glEnd();

	/*
	Bottom Face
	*/
	glBindTexture(GL_TEXTURE_2D,textureObjects2d[5]);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0,1);
	glVertex3f(-10,-10,-10);
	glTexCoord2f(0,0);
	glVertex3f(-10,-10,10);
	glTexCoord2f(1,0);
	glVertex3f(10,-10,10);
	glTexCoord2f(1,1);
	glVertex3f(10,-10,-10);
	glEnd();

	/*
	Reactive depth buffer
	*/
	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
}




/*
Main Display Function
*/
void
onDisplay(void)
{
	int hudY;
	
	/*
	Set projection matrix for 100 degree FOV
	*/
	GLfloat w = (GLfloat)glutGet(GLUT_WINDOW_WIDTH),h = (GLfloat)glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100.0,w/h,1,100);
	
	/*
	Set modelview matrix for camera 
	*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(cameraPhi,1,0,0);
	glRotatef(cameraTheta,0,1,0);
	

	/*
	No need to clear the color buffer, the skybox fills the whole screen
	Shiny objects in cube environment mode use depth buffer, so we do need to clear that one
	*/
	glClear(GL_DEPTH_BUFFER_BIT);
	
	
	/*
	Draw skybox
	*/
	if(skyboxMode == kSkyboxMode2d)
	{
		drawSkybox2d();
	}
	else
	{
		drawSkyboxCubeEnvironment();
	}

	/*
	Display the HUD, if so inclined
	*/
	if(hudMode)
	{
		hudBegin();
		glColor3f(1,0,0);

		hudY = 16;
		
		if(skyboxMode == kSkyboxMode2d)
		{
			hudDisplayString(0,hudY,GLUT_BITMAP_HELVETICA_12,"Render Mode - 2d Texture");
		}
		else
		{
			hudDisplayString(0,hudY,GLUT_BITMAP_HELVETICA_12,"Render Mode - Cubic Environment Map");
			hudY+=16;
			switch(objectType)
			{
			case kTorus :
				hudDisplayString(0,hudY,GLUT_BITMAP_HELVETICA_12,"Object Type - Torus");
				break;
			case kSphere :
				hudDisplayString(0,hudY,GLUT_BITMAP_HELVETICA_12,"Object Type - Sphere");
				break;
			case kTeapot :
				hudDisplayString(0,hudY,GLUT_BITMAP_HELVETICA_12,"Object Type - Teapot");
				break;
			case kCube :
				hudDisplayString(0,hudY,GLUT_BITMAP_HELVETICA_12,"Object Type - Cube");
				break;
			case kNoObject :
				hudDisplayString(0,hudY,GLUT_BITMAP_HELVETICA_12,"Object Type - None");
				break;
			};
		}
		
		glColor3f(1,1,1);
		hudEnd();
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

/*
Timer function rotates shiny object
*/
void
onTimer(int value)
{
	if(rotateObject)
	{
		objectAngle=(objectAngle+1)%360;
	}
	glutTimerFunc(10, onTimer, 0);
}


/*
Mouse move functions used to implement "free look" camera
*/
void
onMouseMoveButtonDown(int x,int y)
{
	cameraTheta += (float)(x-mouseDownX);
	cameraPhi += (float)(y-mouseDownY);

	cameraPhi = max(-90,min(cameraPhi,90));

	mouseDownX = x;
	mouseDownY = y;
}


void
onMouseDown(int b,int s,int x,int y)
{
	if (s == GLUT_DOWN)
	{
		mouseDownX = x;
		mouseDownY = y;
		glutMotionFunc(onMouseMoveButtonDown);
	}
	else
	{
		glutMotionFunc(0);
	}
}

/*
Handle cursor keys
*/
void
onSpecialKey(int key,int x,int y)
{
	switch(key)
	{
	case GLUT_KEY_RIGHT:
		cameraTheta = cameraTheta+5;
		while (cameraTheta > 360) cameraTheta -= 360;
		break;
	case GLUT_KEY_LEFT:
		cameraTheta = cameraTheta-5;
		while (cameraTheta < 0) cameraTheta += 360;
		break;
    case GLUT_KEY_UP:
		cameraPhi = cameraPhi+5;
		if (cameraPhi > 90) cameraPhi = 90;
		break;
	case GLUT_KEY_DOWN:
		cameraPhi = cameraPhi-5;
		if (cameraPhi < -90) cameraPhi = -90;
		break;
	};
}

/*
Handle typed characters
*/
void
onKey(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 'M':
	case 'm':
		if(cubeMapAvailable)
		{
			skyboxMode = (skyboxMode==kSkyboxMode2d) ? kSkyboxModeCubeEnvironment : kSkyboxMode2d;
		}
		break;

	case 'r':
	case 'R':
		rotateObject = !rotateObject;
		break;
	
	case 'o':
	case 'O':
		objectType = (objectType+1)%kObjectTypeCount;
		break;

	case 'h':
	case 'H':
		hudMode = !hudMode;
		break;
	};
}


/*
Loads a Windows BMP file onto the given texture target
*/
GLboolean
loadImage(GLenum target,const char *fileName)
{
	int i,topRow,bottomRow;
	AUX_RGBImageRec *image = auxDIBImageLoad(fileName);
	if(image!=0)
	{
		/*
		BMP files are upside down, so flip pixels
		*/
		topRow = 0;
		bottomRow = 3*image->sizeX*(image->sizeY-1);
		while (topRow < bottomRow)
		{
			for (i=0;i<3*image->sizeX;i++)
			{
				SWAP_BYTES(image->data[bottomRow+i],image->data[topRow+i]);
			}
			bottomRow -= 3*image->sizeX;
			topRow += 3*image->sizeX;
		}	

		glTexImage2D(target,0,3,image->sizeX,image->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,image->data);

		free(image->data);
		free(image);

		return GL_TRUE;
	}
	else
	{
		printf("Could not load texture file %s\n",fileName);
		return GL_FALSE;
	}
}

/*
Set up skybox rendering
*/
GLboolean
initSkybox(void)
{
	int i;
	GLint clampMode;
	GLboolean succeeded = GL_TRUE;
	char *imageFiles[] = {"image1.bmp","image2.bmp","image3.bmp","image4.bmp","image5.bmp","image6.bmp"};
	
	/*
	Allocate texture objects
	*/
	glGenTextures(6,textureObjects2d);
	glGenTextures(1,&textureObjectCubeMap);

	/*
	Let OpenGL normalize normals
	*/
	glEnable(GL_NORMALIZE);

	/*
	Use GL_CLAMP_TO_EDGE if we can
	*/
	if (glutExtensionSupported("GL_EXT_texture_edge_clamp"))
	{
		clampMode = GL_CLAMP_TO_EDGE;
	}
	else
	{
		clampMode = GL_CLAMP;
	}

	/*
	Generate 2d textures
	*/
	for (i=0;i<6;i++)
	{
		glBindTexture(GL_TEXTURE_2D,textureObjects2d[i]);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clampMode);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clampMode);
		succeeded = succeeded && loadImage(GL_TEXTURE_2D,imageFiles[i]);
	}

	
	/*
	Generate cube map, if supported
	*/
	if(glutExtensionSupported("GL_ARB_texture_cube_map"))
	{
		cubeMapAvailable = 1;
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB,textureObjectCubeMap);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

		succeeded = succeeded && loadImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,imageFiles[0]);
		succeeded = succeeded && loadImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,imageFiles[1]);
		
		succeeded = succeeded && loadImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,imageFiles[2]);
		succeeded = succeeded && loadImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,imageFiles[3]);
		
		succeeded = succeeded && loadImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,imageFiles[4]);
		succeeded = succeeded && loadImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,imageFiles[5]);
		
	}
	else
	{
		cubeMapAvailable = 0;
	}
	return succeeded;

}

/*
Shutdown code 
*/
void
shutdownSkybox(void)
{
	/*
	Deallocate texture objects
	*/
	glDeleteTextures(6,textureObjects2d);
	glDeleteTextures(1,&textureObjectCubeMap);
}


/*
hudBegin()/hudEnd() modeled after glBegin()/glEnd()
*/
void
hudBegin(void)
{
	GLfloat w,h;

	/*
	No world transform in HUD mode
	*/
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	

	/*
	Switch to orthographic projection
	*/
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	w = (GLfloat)glutGet(GLUT_WINDOW_WIDTH);
	h = (GLfloat)glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0,w,0,h,-1.0,1.0);

	/*
	Save current attributes and disable texturing
	*/
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);

	if(cubeMapAvailable)
	{
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	}
}

void
hudEnd(void)
{
	/*
	Pop attributes and matrices
	*/
	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

/*
HUD text display code
Between hudBegin() and hudEnd(), call hudDisplayString() to display text at x,y with 0,0 at the upper left-hand corner
*/
void 
hudDisplayString(int x,int y,void *font,const char *str)
{
	int i,l=strlen(str);
	GLint h = glutGet(GLUT_WINDOW_HEIGHT);

	glRasterPos2i(x,h-y);

	for(i=0;i<l;i++)
		glutBitmapCharacter(font,str[i]);
}


int
main(int argc, char **argv)
{
	
	/*
	Initialize GLUT Window
	*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Game Gems II Skybox Demo");
	
	/*
	Setup GLUT callbacks
	*/
	glutDisplayFunc(onDisplay);
	glutMouseFunc(onMouseDown);
	glutSpecialFunc(onSpecialKey);
	glutKeyboardFunc(onKey);
	glutTimerFunc(10, onTimer, 0);

	/*
	Initialize
	*/
	if(!initSkybox())
	{
		shutdownSkybox();
		return 0;
	}
	
	/*
	Print Instructions
	*/
	printf("Game Gems II Skybox Demo\n");
	printf("Keys:\n");
	printf("Arrow Keys - move camera\n");
	if(cubeMapAvailable)
	{
		printf("M - toggle render mode (2d texture/cube environment)\n");
		printf("R - toggle object rotation in cube environment mode (on/off)\n");
		printf("O - cycle object type in cube environment mode\n");
	}
	else
	{
		printf("Cubic Environment Mapping Not Available!\n");
	}
	printf("H - toggle hud mode (on/off)\n");
	printf("Click and drag mouse for free-look camera\n");
	
	/*
	Run, Lola, Run
	*/
	glutMainLoop();
	
	/*
	Deinitialize
	*/
	shutdownSkybox();
	return 0;             
}

