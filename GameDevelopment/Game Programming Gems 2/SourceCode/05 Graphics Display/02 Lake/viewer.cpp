/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Cartoon Shading in Real-Time

Copyright (c) 2001, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/

//////////////////////////////////////////////////////////////////////////
// viewer.cpp
//
//////////////////////////////////////////////////////////////////////////

#include "camera.h"
#include "Cartoon.h"
#include "SurfOfRevolution.h"
#include "viewer.h"

#include <stdio.h>
#include <stdlib.h>

/*****************************************************/
// Cartoon variables
Cartoon cartoon;
Mesh mesh;
//for painter
static GLuint texName;

extern void BuildSurfaceOfRevolution(Mesh *mesh);
/*****************************************************/

Camera Cam;
int OldMouseX, OldMouseY;
int Mode=0;
vector3 ModelCenter(0,0,0);
float TRANS=0.003f;  //translation sensitivity
int LeftButtonDown=0;
int Wireframe = 0;
//used to set the 'active' window that is responding to events

int sceneChanged=TRUE;

//////////////////////////////////////////////////////////////////////////
//  myReshape
//  Sets matrices up correctly when the window has been resized
//
//
//////////////////////////////////////////////////////////////////////////

void myReshape(int w, int h)
{
    glViewport(0,0,w,h); //last stage in the pipe
                         //after proj and modelview
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, (GLfloat)w/(GLfloat)h, 0.01, 1000);
    Cam.Perspective(65, (GLfloat)w/(GLfloat)h, 0.01f, 1000);

}

void DrawMesh()
{
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	static int stride = sizeof(vector3) + sizeof(vector3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);	

	char *bytePtr = (char*) mesh.GetVertexPtr();
	glColor3f(0.5, 0.5, 0.5);

	glVertexPointer(3, GL_FLOAT, stride, bytePtr);
	glNormalPointer(GL_FLOAT, stride, bytePtr + sizeof(vector3));

	glDrawElements(GL_TRIANGLES, mesh.GetNumFaces() * 3, GL_UNSIGNED_INT, mesh.GetFacePtr());

}

//////////////////////////////////////////////////////////////////////////
//	DrawScene
//  
//////////////////////////////////////////////////////////////////////////
		
void DrawScene()
{
    if(Wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	cartoon.CalculateTextureCoordinates(&mesh);

	cartoon.RenderCartoon(&mesh);
  
}

//////////////////////////////////////////////////////////////////////////
//	myDisplay
//  Loads the camera from the camera model onto the OpenGL matrix stack
//  Draws the scene and swaps the buffers
//////////////////////////////////////////////////////////////////////////

void myDisplay(void)
{
	glClearColor(.6f,.6f,.6f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    GLfloat M[16];
	glLoadIdentity();
    glLoadMatrixf(Cam.GetViewMatrix(M));

    DrawScene();
    glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
//  InitCartoon
//
//  Sets up the Cartoon class
//////////////////////////////////////////////////////////////////////////

void InitCartoon(void)
{
	// Set up Painter
	cartoon.BuildTexture();

}

//////////////////////////////////////////////////////////////////////////
//  InitializeScene
//
//  Calls various initialization routines.  
//////////////////////////////////////////////////////////////////////////

void InitializeScene(void)
{
	BuildSurfaceOfRevolution(&mesh);
	
	InitCartoon();
	
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	//  glEnable(GL_AUTO_NORMAL);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
}

//////////////////////////////////////////////////////////////////////////
//  KeyboardHandler
//  Handles all keyboard events, toggles state of GUI
//
//////////////////////////////////////////////////////////////////////////

void KeyboardHandler(unsigned char key, int x, int y)
{
    switch(key) {
    case 27:
		printf("Quitting...\n");
        exit(0);
        break;
	case 'w':
		Wireframe = !Wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
    default: 
        printf("Key %d not supported\n",key);
        break;
    }
	sceneChanged=TRUE;
    glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////
//  MouseInputHandler
//
//  Handles mouse input.  Currently only starts the inertial system
//  trackball.
//////////////////////////////////////////////////////////////////////////

void MouseInputHandler(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON) {
        if(state==GLUT_DOWN) {
            OldMouseX=x;OldMouseY=y;
            LeftButtonDown=1;
        }
        else if(state==GLUT_UP) {
            LeftButtonDown=0;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//
//  MouseMoveHandler
//  Handles all mouse movement
//////////////////////////////////////////////////////////////////////////

void MouseMoveHandler(int x, int y)
{
    if(LeftButtonDown) {
        float RelX=x-OldMouseX;
        float RelY=y-OldMouseY;
        
        OldMouseX=x;OldMouseY=y;
    
        float RotX = RelY*.005;
        float RotY = RelX*.005;

        Cam.TrackballRotate(ModelCenter, -RotX,0);
        Cam.TrackballRotate(ModelCenter, RotY, 1);
    }
	sceneChanged=TRUE;
    glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////
//  InitGLandPointAtBoundingBox
//  Handles all of the OpenGL initialization and setting up glut
//////////////////////////////////////////////////////////////////////////

void InitGLandPointAtBoundingBox(float minx, float miny, float minz, 
              float maxx, float maxy, float maxz)
{
    //Set initial view
    vector3 ModelMin(minx, miny, minz), ModelMax(maxx, maxy, maxz);
    ModelCenter = (ModelMax+ModelMin)*0.5;
    vector3 ViewPos = ModelCenter + ((ModelMax-ModelCenter)*1.5);
    Cam.Lookat(ViewPos, ModelCenter, vector3(0,1,0));
    
    //Set translation scales
    vector3 Dim = ModelMax-ModelMin;
    float MaxDim = Dim.x;
    if(Dim.y > MaxDim) MaxDim = Dim.y;
    if(Dim.z > MaxDim) MaxDim = Dim.z;
    TRANS*=(MaxDim*0.5);


   //Now setup openGL stuff
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(500,500);
	glutCreateWindow("Cartoon Renderer");

    //Initialize Scene
    InitializeScene();

    glutMouseFunc(MouseInputHandler);
    glutMotionFunc(MouseMoveHandler);
    glutKeyboardFunc(KeyboardHandler);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(myDisplay);

	glutInitWindowPosition(600,100);
	glutInitWindowSize(300,500);

    glutMainLoop();
}

//////////////////////////////////////////////////////////////////////////
//  main
//
//  Starts
//////////////////////////////////////////////////////////////////////////

void main()
{
		InitGLandPointAtBoundingBox(-1.5, 1.5, -1.5, 1.5, -1.5, 1.5);
}