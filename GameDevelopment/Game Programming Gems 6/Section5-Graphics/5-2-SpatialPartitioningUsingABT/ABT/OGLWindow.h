/*******************************************************************\
| OGLWindow.h
|
|--------------------------------------------------------------------
| Copyright 2005. Martin Fleisz.
| All rights reserved.
|
|--------------------------------------------------------------------
| CREATED:		2005/08/20
| AUTHOR:		Martin Fleisz
|
|--------------------------------------------------------------------
| DESCRIPTION:
|
| OpenGL Framework header (based on Jeff Molofee's OpenGL framework)
\********************************************************************/

#ifndef OGLWINDOW_H
#define OGLWINDOW_H

#include "Core.h"
#include "ABT.h"

class cOGLWindow
{
	public:

		cOGLWindow(HINSTANCE p_Instance);
		~cOGLWindow();

		boolean	create(TCHAR *p_Title, int32 p_Width, int32 p_Height, boolean p_Fullscreen);
		void	destroy();
		boolean registerClass();
		void	unregisterClass();
	
		// action happens here
		boolean init();
		void	update(uint32 p_Milliseconds);
		void	draw();

		// access to the window's handle
		operator HWND() const {	return m_hWnd; }

		// public data
		boolean			m_bIsFullScreen;	// FullScreen?
		boolean			m_bIsVisible;		// Window Visible?
		const TCHAR*	m_szClassName;		// Application ClassName

	protected:
		
		boolean	changeScreenResolution(uint32 p_Width, uint32 p_Height);
		void	resizeGL(uint32 p_Width, uint32 p_Height);

		// windows message proc
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		boolean			m_bKeyDown[256];	// Holds TRUE / FALSE For Each Key
		HINSTANCE		m_hInstance;		// Application Instance
		HWND			m_hWnd;				// Window handle
		HDC				m_hDC;				// device context
		HGLRC			m_hRC;				// gl context
		int32			m_iWidth, m_iHeight;// Window size
		uint32			m_iFrames;
		uint32			m_iLastFPS;
		uint32			m_iNumTriangles;

		cABT			m_ABT;
};


#endif	// OGLWINDOW_H
