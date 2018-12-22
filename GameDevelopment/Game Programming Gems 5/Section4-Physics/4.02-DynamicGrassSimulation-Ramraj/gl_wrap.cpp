//----------------------------------------------------------------------------
//
//gl_wrap.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

#include "gl_wrap.h"

//··········································································//
//    Source :: Definitions
//··········································································//

#define GL_PER_ANGLE 45.0f
#define GL_END_DIST 100.0f

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

//··········································································//
//    Source :: External Function Implementation
//··········································································//

PIXELFORMATDESCRIPTOR gl_get_pfd(int bpp)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof (PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bpp,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		16,
		0, 0,
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};

	return pfd;
}

bool gl_create_wnd( const HWND & hWnd, const HDC & hDC, HGLRC & hRC)
{
	hRC = wglCreateContext(hDC);
	if(!hRC)
		return false;

	if(wglMakeCurrent(hDC,hRC) == FALSE)
	{
		wglDeleteContext(hRC);
		hRC = 0;
		return false;
	}
	
	return true;
}

void gl_resize_wnd(GLsizei width, GLsizei height)
{
	if(!width || !height)
		return;

	glViewport( 0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( GL_PER_ANGLE, (GLfloat)width/(GLfloat)height, 0.01f, GL_END_DIST);
	glMatrixMode(GL_MODELVIEW);
}

void gl_destroy_wnd( const HWND & hWnd, const HDC & hDC, HGLRC & hRC)
{
	if(hWnd && hDC)
	{
		wglMakeCurrent(hDC,0);
		if(hRC)
		{
			wglDeleteContext(hRC);
			hRC = 0;
		}
	}
}

//··········································································//
//    Source :: External Class Implementation
//··········································································//

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//gl_wrap.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
