/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Window.h
 *    Desc: declaration of a generic window class.  Used primarily
 *          to create the main application window
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _WINDOW_H
#define _WINDOW_H

#include <windows.h>

#include <list>
#include <string>

#include "GameErrors.h"


/**
 * cWindow 
 * Window encapsulator
 */
class cWindow  
{
protected:

	// Width and height of the client area
	int m_Width, m_Height;

	// Handle to the window
	HWND m_hWnd;

	// Name of the window 
	std::string m_name;

	// True if the window is active	
	bool m_bActive;

	static cWindow* m_pGlobalWindow;

public:
	cWindow( 
		int width, 
		int height, 
		const char* name = "Default window name" );
	~cWindow();

	virtual LRESULT WndProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam );

	virtual void RegisterClass( WNDCLASSEX* pWc = NULL );
	virtual void InitInstance();

	HWND GetHWnd()
	{
		return m_hWnd;
	}

	bool IsActive()
	{ 
		return m_bActive; 
	}

	// True -> there are messages in the queue for us to pump
	bool HasMessages();

	// res_AllGood -> continue running
	// res_False -> got a WM_QUIT.  Stop the application.
	eResult Pump();

	static cWindow* GetMainWindow()
	{ 
		return m_pGlobalWindow; 
	}

};

inline cWindow* MainWindow()
{ 
	return cWindow::GetMainWindow(); 
}

/**
 * cFullScreenWindow represents a
 * fullscreen window.  The only real
 * difference here is that it uses
 * the WM_POPUP window style.
 */
class cFullscreenWindow : public cWindow
{
public:
	cFullscreenWindow( int width, int height, const char* name = "Default window name" );
	virtual void InitInstance();
};

#endif //_WINDOW_H
