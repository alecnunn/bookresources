/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Window.cpp
 *    Desc: definition of a generic window class.  Used primarily
 *          to create the main application window
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "Window.h"
#include "GraphicsLayer.h"
#include "Application.h"

using namespace std;

cWindow* cWindow::m_pGlobalWindow = NULL;

/**
 * We can't unwind our stack across the dll boundary
 * into kernel32.dll or any other system dll's.  
 * because of this we must catch any errors that
 * occur in wndproc and then re-throw them inside
 * the message pump.
 */
cGameError* g_pThrownError = NULL;


LRESULT CALLBACK GlobalWndProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam )
{
	assert( MainWindow() );

	return MainWindow()->WndProc( hWnd, uMsg, wParam, lParam );
}



cWindow::cWindow( int width, int height, const char* name ) :
	m_Width( width ),
	m_Height( height ),
	m_hWnd( NULL )
{
	/**
	 * Only one primary window should be created.
	 */
	if( m_pGlobalWindow )
	{
		throw cGameError( "Window object already instantiated!\n" );
	}

	m_pGlobalWindow = this;
	m_bActive = false;


	// copy the name over
	m_name = string( name );
}

cWindow::~cWindow()
{
	DestroyWindow( m_hWnd );
	m_pGlobalWindow = NULL;
}


LRESULT cWindow::WndProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam )
{
	/**
	 * If we get another message after a thrown
	 * error, return immediately
	 */
	if( g_pThrownError )
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);	
	}


	bool bRunDefault = false;

	try
	{
		switch( uMsg ) 
		{
		case WM_CREATE:
			{
				MainWindow()->m_hWnd = hWnd;
				MainWindow()->m_bActive = true;
				break;
			}
		case WM_PAINT:
			{
				// don't do anything.
				ValidateRect( hWnd, NULL );
				break;
			}
		case WM_KEYDOWN:
			{
				switch( wParam )
				{
				// Quit if the escape key is pressed
				case VK_ESCAPE:
					{
						PostQuitMessage(0);
						break;
					}
				default:
					{
						break;
					}
				}
			}

		case WM_SETFOCUS:
			{
				if( Input() )
				{
					Input()->SetFocus();
				}
				break;
			}
		case WM_KILLFOCUS:
			{
				if( Input() )
				{
					Input()->KillFocus();
				}
				break;
			}
		case WM_ERASEBKGND:
			{
				break;
			}
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
		default:
			bRunDefault = true;
			break;
		}	
	}
	catch( cGameError& err )
	{
		g_pThrownError = new cGameError( err );
		return 0;
	}

	if( bRunDefault )
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


void cWindow::RegisterClass( WNDCLASSEX* pWc )
{
	WNDCLASSEX wcex;
	if( pWc )
	{
		wcex = *pWc;
	}
	else
	{
		// fill in some default values
		wcex.cbSize = sizeof(WNDCLASSEX); 

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= GlobalWndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= AppInstance();
		wcex.hIcon			= 0;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= m_name.c_str();
		wcex.hIconSm		= 0;
	}

	if( 0 == RegisterClassEx(&wcex) )
	{
		throw cGameError("[cWindow::RegisterClass]: Registering window class failed!\n");
	}
}

void cWindow::InitInstance()
{

	HINSTANCE hi = AppInstance();

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = m_Width;
	rc.bottom = m_Height;
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	/**
	 * Usually we capture the return value of CreateWindow,
	 * as it is the window handle for our window. However,
	 * before CreateWindow finishes, WM_CREATE is called
	 * on our window, and we just capture the window handle
	 * there.
	 */
	CreateWindow( 
	   m_name.c_str(), 
	   m_name.c_str(), 
	   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
	   0, 
	   0, 
	   rc.right - rc.left, 
	   rc.bottom - rc.top, 
	   NULL, 
	   NULL, 
	   AppInstance(), 
	   NULL);

   if ( !m_hWnd )
   {
		throw cGameError("[cWindow::InitInstance]: Creating window failed!\n");
   }

   // Draw
   ShowWindow( m_hWnd, SW_SHOW );
   UpdateWindow( m_hWnd );
}


bool cWindow::HasMessages()
{
	MSG msg;
    if( 0 == PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE ) )
	{
		// There weren't any messages
		return false;
	}
	return true;
}

eResult cWindow::Pump()
{
	MSG msg;
	PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );

	if( WM_QUIT == msg.message )
		return resFalse;

	TranslateMessage( &msg );
    DispatchMessage( &msg );

	/**
	 * Check to see if an error occured inside the pump
	 */
	if( g_pThrownError )
	{
		cGameError out = *g_pThrownError;
		delete g_pThrownError;
		throw out;
	}


	return resAllGood;
}



//==========--------------------------  cFullscreenWindow

cFullscreenWindow::cFullscreenWindow( int width, int height, const char* name ) :
	cWindow( width, height, name )
{
}

void cFullscreenWindow::InitInstance()
{
	CreateWindow( 
	   m_name.c_str(), 
	   m_name.c_str(), 
	   WS_POPUP,
	   0, 0, m_Width, m_Height, 
	   NULL, 
	   NULL, 
	   AppInstance(), 
	   NULL);

   if ( !m_hWnd )
   {
		throw cGameError("[cWindow::InitInstance]: Creating window failed!\n");
   }
   UpdateWindow( m_hWnd );
}
