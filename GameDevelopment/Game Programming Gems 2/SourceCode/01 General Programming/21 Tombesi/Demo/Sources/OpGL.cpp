/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "std.h"
#include "OpGl.h"


#include "Skin.h"
extern _Skin* pSkin;

//==============================================================================================================
//////////////////////////////////////////////////////////////////////////////
// Initalize The OpenGL System
//////////////////////////////////////////////////////////////////////////////
int InitGL(GLvoid)													// All Setup For OpenGL Goes Here
{																			
	// Modify This Section To Suite Your Needs For Your OpenGL Scene
	//////////////////////////////////////////////////////////////////////////////
	glShadeModel(GL_SMOOTH);												// Enable Smooth Shading
	glClearColor(0.7f,0.8f,0.8f,0.5f);
	glClearDepth(1.0f);														// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);												// Enables Depth Testing
	
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glLoadIdentity();														// Reset The Projection Matrix
	
	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix
	glLoadIdentity();														// Reset The Modelview Matrix
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	return TRUE;															// Initialization Went OK
}

//==============================================================================================================
//////////////////////////////////////////////////////////////////////////////
// Render The OpenGL Scene:
//			 Here's Where We Do All The Drawing
//////////////////////////////////////////////////////////////////////////////
#include "resource.h"
int DrawGLScene(GLvoid)										
{																			
	static GLfloat a=0;

		//FPS data update
		static double fps=0;
		static double temp=0;
		static char buf[30]={char(0)};
		static uint frames=0;
		if (frames==500)
		{
			fps = ( 500000.0f / (t.time-temp)  );
			temp = t.time;
			sprintf( buf, "FPS: %d", (int)fps );

				HWND hFPS = GetDlgItem (hDLGWnd,IDC_FPS);
				SendMessage (hFPS, WM_SETTEXT,0,(LPARAM)(LPCTSTR)buf);

			frames=0;
		}
		frames++;
	
	// Clear Screen And Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

	//update the Character
	pSkin->Update(ulong(t.time));

	glPushMatrix();

	//world origin position
	glTranslatef(0,-350,-1450);	

		glRotatef(25,1,0,0);
				
			//Paint the GRID
			DisplayGrid(1600,darkgrey);
			//Set the Light position
			static const GLfloat pos[4]={50,100,120,0};
			glLightfv(GL_LIGHT0,GL_POSITION,pos);
			
		//Object Rotation
		glRotatef(a,0,-1,0);

		////////////////////////
			//Paint the SKELETON
			if (skel)
			{
				glDisable(GL_LIGHTING);
				DisplaySkel(pSkin->RootBone);
				glEnable(GL_LIGHTING);
			}

			//Paint the MESH
			else DisplayMesh(pSkin);		
			
			//Paint the NORMALS
			if (normals)
				DisplayNormals(pSkin,8);				
		////////////////////////
	
	glPopMatrix();

	//Object Rotation increment for each frame
	if (rotate)
		a+=0.1f;
		
	return TRUE;
}

//==============================================================================================================
//////////////////////////////////////////////////////////////////////////////
// Resize The OpenGL Scene (Window)
//////////////////////////////////////////////////////////////////////////////
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)							// Resize And Initialize The GL Window
{																			
	if (height==0)															// Prevent A Divide By Zero By
	{																		
		height=1;															// Making Height Equal One
	}																		
																			
	glViewport(0,0,width,height);											// Reset The Current Viewport
																			
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glLoadIdentity();														// Reset The Projection Matrix
	// Modify The Projection Matrix To Suite Your Needs
	//////////////////////////////////////////////////////////////////////////////
	gluPerspective(50.0f, (GLfloat)width/(GLfloat)height, 300, 3300);		// Calculate The Aspect Ratio Of The Window
	//glFrustum(-40,40,-40,40,50,600);

	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix

}
