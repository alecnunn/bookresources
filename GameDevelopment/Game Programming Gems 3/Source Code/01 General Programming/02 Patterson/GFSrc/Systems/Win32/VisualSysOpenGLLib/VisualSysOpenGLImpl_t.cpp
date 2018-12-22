/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSysOpenGLImpl_t.cpp: implementation of the VisualSysOpenGLImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisualSysOpenGLImpl_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/Win32/WindowSysWin32Impl_t.h"
#include "GFInclude/Objects/Box2D_t.h"
#include "GFInclude/Objects/TextBox2D_t.h"
#include "GFInclude/Objects/MenuBox2D_t.h"
#include "OpenGLFont_t.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VisualSysOpenGLImpl_t::VisualSysOpenGLImpl_t()
{
	m_pSystems = 0;
	m_hDC = NULL;
	m_hRC = NULL;
	m_pFont = NULL;
}

VisualSysOpenGLImpl_t::~VisualSysOpenGLImpl_t()
{
	Shutdown();
}

bool VisualSysOpenGLImpl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	m_bWindowed = TRUE;
    m_pFont = new OpenGLFont_t( _T("Arial"), 12, OPENGLFONT_BOLD );

	// get the window sys
	WindowSys_t *pWS = m_pSystems->GetWindowSys();
	int nWidth;
	int nHeight;
	pWS->GetWindowInfo( nWidth, nHeight );

	if( !m_bWindowed )
	{
		//get rid of the mouse cursor
		ShowCursor(FALSE);
	}

	// we know this is the Win32 implementation
	WindowSysWin32Impl_t *pWin = (WindowSysWin32Impl_t *)pWS;
	HRESULT hr = CreateDevice(pWin->m_hWnd, nWidth, nHeight, 16);
	if(FAILED(hr))
	{
		return false;
	}

    // Initialize device-dependent objects
    if( InitDeviceObjects() )
	{
		// Restore device-dependent objects
        if( RestoreDeviceObjects() )
		{
            return true;
		} else {
			return false;
		}
	}
	return false;
}

void VisualSysOpenGLImpl_t::Shutdown()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	if( m_pFont )
	{
		delete m_pFont;
		m_pFont = NULL;
	}

	if(m_hWnd){

		if(m_hDC){
			wglMakeCurrent(m_hDC, 0);
			if(m_hRC){
				wglDeleteContext(m_hRC);
				m_hRC = NULL;
			}
			ReleaseDC(m_hWnd, m_hDC);
			m_hRC = NULL;
		}

	}

	if( !m_bWindowed )
	{
		// restore display
		ChangeDisplaySettings(NULL, 0);
		// restore cursor
		ShowCursor(TRUE);
	}
}

bool VisualSysOpenGLImpl_t::InitDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->InitDeviceObjects();
	}
	return true;
}

bool VisualSysOpenGLImpl_t::RestoreDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->RestoreDeviceObjects();
	}
	return true;
}

bool VisualSysOpenGLImpl_t::InvalidateDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->InvalidateDeviceObjects();
	}
	return true;
}

bool VisualSysOpenGLImpl_t::DeleteDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->DeleteDeviceObjects();
	}
	return true;
}

bool VisualSysOpenGLImpl_t::BeginRender()
{
	WindowSys_t *pWS = m_pSystems->GetWindowSys();
	int nWidth;
	int nHeight;
	pWS->GetWindowInfo( nWidth, nHeight );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Push the neccessary Matrices on the stack
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, nWidth, nHeight, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();

	// Push the neccessary Attributes on the stack
	glPushAttrib(GL_TEXTURE_BIT|GL_ENABLE_BIT);

	return true;
}

void VisualSysOpenGLImpl_t::EndRender()
{
	m_pFont->DrawText( 2, 2, 0xffffffff, "OpenGL Driver" );

	// Return to previous Matrix and Attribute states. Easy cleanup!
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();

	SwapBuffers(m_hDC);
}

void VisualSysOpenGLImpl_t::WindowMove( int nX, int nY )
{
}

HRESULT VisualSysOpenGLImpl_t::CreateDevice(HWND hWnd,
								  DWORD dwWidth,
								  DWORD dwHeight,
								  DWORD dwDepth )
{
	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
	m_dwDepth = dwDepth;
	m_hWnd = hWnd;

	if( !m_bWindowed )
	{
		DEVMODE dmScreenSettings;					// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= m_dwWidth;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= m_dwHeight;			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= m_dwDepth;				// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL){
			MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
			return FALSE;					// Exit And Return FALSE
		}
	}

	static PIXELFORMATDESCRIPTOR pfd =	// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		(int)m_dwDepth,					// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		16,								// 16Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
	};

	if (!(m_hDC=GetDC(hWnd)))			// Did We Get A Device Context?
	{
		Shutdown();
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;					// Return FALSE
	}

	GLuint pixelFormat;
	if (!(pixelFormat=ChoosePixelFormat(m_hDC,&pfd))) // Did Windows Find A Matching Pixel Format?
	{
		Shutdown();
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;							// Return FALSE
	}

	if(!SetPixelFormat(m_hDC,pixelFormat,&pfd))	// Are We Able To Set The Pixel Format?
	{
		Shutdown();
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;							// Return FALSE
	}

	if (!(m_hRC=wglCreateContext(m_hDC)))	// Are We Able To Get A Rendering Context?
	{
		Shutdown();
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;							// Return FALSE
	}

	if(!wglMakeCurrent(m_hDC,m_hRC))	// Try To Activate The Rendering Context
	{
		Shutdown();
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;							// Return FALSE
	}

	//set our background clearing color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//create a perspective projection
	//glMatrixMode(GL_PROJECTION);						
	//glLoadIdentity();							
	//gluPerspective(45.0f,(GLfloat)m_dwWidth/(GLfloat)m_dwHeight,1.0f,100.0f);

	//switch to our model matrix stack
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	return S_OK;
}

void VisualSysOpenGLImpl_t::RenderBox( Box2D_t *pBox )
{
	m_pFont->DrawBox( pBox->m_fXPos, pBox->m_fYPos, pBox->m_fXSize, pBox->m_fYSize, pBox->m_nColor );
}

void VisualSysOpenGLImpl_t::RenderTextBox( TextBox2D_t *pTextBox )
{
	if( pTextBox->m_bDrawBox )
	{
		m_pFont->DrawBox( pTextBox->m_fXPos, pTextBox->m_fYPos, pTextBox->m_fXSize, pTextBox->m_fYSize, pTextBox->m_nBoxColor );
	}
	if( pTextBox->m_bDrawText )
	{
		m_pFont->DrawText( pTextBox->m_fXPos, pTextBox->m_fYPos, pTextBox->m_nTextColor, pTextBox->m_Text.c_str() );
	}
}

void VisualSysOpenGLImpl_t::RenderMenuBox( MenuBox2D_t *pMenuBox )
{
	int nChoice = pMenuBox->m_nChoice;
	int nCount = 0;
	TextBox2D_Vector_t::iterator iTBV;
	for( iTBV = pMenuBox->m_TextBoxVector.begin(); iTBV != pMenuBox->m_TextBoxVector.end(); ++iTBV )
	{
		TextBox2D_t &TB = *iTBV;
		if( TB.m_bDrawBox )
		{
			TB.m_nBoxColor = (nChoice == nCount) ? pMenuBox->m_nBoxHighlightColor : pMenuBox->m_nBoxNormalColor;
			m_pFont->DrawBox( pMenuBox->m_fXPos+TB.m_fXPos, pMenuBox->m_fYPos+TB.m_fYPos, TB.m_fXSize, TB.m_fYSize, TB.m_nBoxColor );
		}
		if( TB.m_bDrawText )
		{
			TB.m_nTextColor = (nChoice == nCount) ? pMenuBox->m_nTextHighlightColor : pMenuBox->m_nTextNormalColor;
			m_pFont->DrawText( pMenuBox->m_fXPos+TB.m_fXPos, pMenuBox->m_fYPos+TB.m_fYPos, TB.m_nTextColor, TB.m_Text.c_str() );
		}
		nCount++;
	}
}

void VisualSysOpenGLImpl_t::RenderStats()
{
}

