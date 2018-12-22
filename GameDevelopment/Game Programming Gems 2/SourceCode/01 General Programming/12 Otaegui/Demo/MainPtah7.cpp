/************************************************************
 * Gamemain.cpp - Generic Game V2.16.001 con DirectX 7
 * 
 * (c) Copyright 1999-2001 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/               

/*
		'If unix is the face of the future I wanna go back to quill pens.'
			
						-Joseph Snipp
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <dinput.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>

#include "mainptah7.h"
#include "input.hpp"
#include "blitbuf.hpp"
#include "support.hpp"

#include "stack.h"
#include "ingame.h"
#include "main.h"

// Control Globals
BOOL	bActive;  				// Esta activa la aplicacion?
BOOL 	bInicio=TRUE;			// Esta iniciandose el thread del juego?
BOOL	bFin=FALSE;				// Esta finalizandose el thread del juego?
BOOL	bSuspended=FALSE;		// Estado del thread principal
HWND	hwnd;					// Id de la ventana
HINSTANCE g_hInstance;		
HANDLE	hMainThread;			// Handle del thread principal
HANDLE	task_wakeup_event;		// Evento para Inter Thread Ccmmunication

// Reload functions Stack
c_stack gReloadSurfacesStack = c_stack( sizeof( CallbackT ) );

// Language for all system messages
int idioma=0;					// El Idioma 0 Castellano 1 Ingles 2 Portugues

// Game-executing-thread creation functions
DWORD WINAPI
ThreadPrincipal( LPVOID arg1 )
{
	fplog( "ini ThreadPrincipal()\n" );

	RunGame();		
	bFin=TRUE;

	// Close everything
	PostMessage(hwnd, WM_CLOSE, 0, 0);

	fplog( "fin ThreadPrincipal()\n" );

	return 0;
}; 

/* Windows-executable required functions */

/*
 * finiObjects
 *
 * finished with all objects we use; release them
 */
static void
finiObjects( void )
{
} /* finiObjects */

long FAR PASCAL
WindowProc( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{
	char szTemp[512];

    switch( message )
    {
    case WM_ACTIVATEAPP:

		fplog( "========> ACTIVATEAPP\n" );

		bActive = (BOOL)wParam && GetForegroundWindow() == hWnd;

		fplog( "bActive %d bInicio %d bFin %d bSuspended %d\n", bActive, bInicio, bFin, bSuspended );

		// If the application is reactivated
		if( bActive && !bInicio && bSuspended)
		{
			fplog( "Application reactivated...\n" );

			SetWindowText(hWnd, TITLE);

			bSuspended=FALSE;

			SetEvent( task_wakeup_event );

			// Restore Lost Surfaces
			fplog( "Forced restoreAll\n");
			restoreAll();
		}

		// If the application is deactivated
		if( !bActive && !bSuspended)
		{
			fplog( "Application deactivated...\n" );

			sprintf( szTemp, "%s - [pause]", TITLE );

			SetWindowText(hWnd, szTemp );

			bSuspended=TRUE;

			ResetEvent( task_wakeup_event );
		}
		
		fplog( "Fin ACTIVATEAPP\n" );

		break;

    case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

    case WM_CREATE:
		break;

    case WM_KEYDOWN:

		switch( wParam )
		{
		case VK_CONTROL:
			g_ShiftStatus|=4;
			break;
		case VK_SHIFT:
			g_ShiftStatus|=1;
			break;
		default:
			ungetch( wParam );
			break;
		}
		break;

	case WM_KEYUP:

		// A Key was released
		switch( wParam )
		{
		case VK_CONTROL:
			g_ShiftStatus^=4;
			break;
		case VK_SHIFT:	
			g_ShiftStatus^=1;
			break;
		}
		break;

    case WM_CLOSE:
		finiObjects();
		PostQuitMessage( 0 );
		exit(0);
		break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);

} /* WindowProc */

/*
 * This function is called if the initialization function fails
 */
BOOL
initFail( HWND hwnd, char *msg )
{
	finiObjects();

	SetCursor( NULL );
	
	if( msg==NULL )
		MessageBox( hwnd, "Error en la inicialización.", TITLE, MB_OK );
	else
		MessageBox( hwnd, msg, TITLE, MB_OK );

    DestroyWindow( hwnd );
    return FALSE;
} /* initFail */

/*
 * doInit - do work required for every instance of the application:
 *          create the window, initialize data
 */
static BOOL
doInit( HINSTANCE hInstance, int nCmdShow )
{
    WNDCLASS            wc;
	DWORD tid;

	// Initialize random numbers
	srand( GetTickCount() );

	// Check whether theres is another copy running
	if( FindWindow( NAME, TITLE ) != NULL )
	{
		fplog( "Hay otra copia corriendo\n" );
			
		return FALSE;
	}

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( hInstance, NULL );
    wc.hCursor = LoadCursor( NULL, IDC_APPSTARTING );
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;

    RegisterClass( &wc );
    
    /*
     * create a window
     */
    hwnd = CreateWindowEx(
	0,
	NAME,
	TITLE,
	WS_POPUP,
	0,
	0,
	GetSystemMetrics(SM_CXSCREEN),
	GetSystemMetrics(SM_CYSCREEN),
	NULL,
	NULL,
	hInstance,
	NULL );

	if( !hwnd )
    {
		return FALSE;
    }

	// Set global g_hInstance
	g_hInstance = hInstance;

	// Disable system cursor
	SetCursor( NULL );

    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );

	// Create ITC event
	task_wakeup_event = 
    CreateEvent( 
        NULL, /* Means no SECURITY ATTRIBUTES */
        TRUE, /* Means Manual Reset ON */
        TRUE, /* Event state = SIGNALED */
        NULL  /* No name */
    );

	// Launch the game-executing-thread
	hMainThread=CreateThread( 0,
		0,
		&ThreadPrincipal,
		0,
		0,
		&tid);

    return TRUE;
} /* doInit */


/*
 * WinMain - initialization, message loop
 */
int PASCAL
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

	if( !doInit( hInstance, nCmdShow ) )
    {
		return FALSE;
    }
									     
    while( GetMessage( &msg, NULL, 0, 0) )
	{
	    TranslateMessage(&msg); 
	    DispatchMessage(&msg);
	}

	return TRUE;

} /* WinMain */
