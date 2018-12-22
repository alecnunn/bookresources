//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			ShadowsDemo
//*				File:				WinMain.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include "stdafx.h"
#include "Frame.h"




//********************************************************************************************************************
//*				GLOBAL
//********************************************************************************************************************

LRESULT WINAPI MsgProc											(	HWND						a_hWnd, 
																	UINT						a_uiMsg, 
																	WPARAM						a_wParam, 
																	LPARAM						a_lParam )
{
	//----------------------------------------------------------|--------------------------------------------MsgProc-|
	// global method
	// Proceeds window essages.

	switch( a_uiMsg )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		case WM_PAINT:
			ValidateRect( a_hWnd, 
						  NULL );
			return 0;
		case WM_ACTIVATE:
			if( WA_INACTIVE != a_wParam )
			{
			}
			return 0;
	}
    return DefWindowProc( a_hWnd, 
						  a_uiMsg, 
						  a_wParam, 
						  a_lParam );
}


HWND CreateTestWindow											(	HINSTANCE					a_hInstance,
																	IGNU32						a_u32WindowWidth,
																	IGNU32						a_u32WindowHeight )
{
	//----------------------------------------------------------|-----------------------------------CreateTestWindow-|
	// global method
	// Creates a new test window.

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), 
					  CS_CLASSDC, 
					  MsgProc, 
					  0L, 
					  0L,  
					  a_hInstance, 
					  NULL, 
					  NULL, 
					  NULL, 
					  NULL, 
					  "Shadows", 
					  NULL };
	RegisterClassEx( &wc );
    HWND hWnd = CreateWindow(  "Shadows",
							   " Game Programming Gems 4 - Combined Depth and ID-Based Shadow Buffers", 
							   WS_BORDER|WS_SYSMENU, 
							   100, 
							   100, 
							   a_u32WindowWidth, 
							   a_u32WindowHeight,
							   NULL, 
							   NULL, 
							   wc.hInstance, 
							   NULL );
	ShowWindow( hWnd, 
				SW_SHOWNORMAL);
	return hWnd;
}


int APIENTRY WinMain											(	HINSTANCE					a_hInstance,
																	HINSTANCE					a_hPrevInstance,
																	LPSTR						a_pCmdLine,
																	int							a_iCmdShow )
{
	//----------------------------------------------------------|--------------------------------------------WinMain-|
	// global method
	// Main function.

	//
	// Start the system ...
	//

	//
	// Examine the command line parameters
	//
	IGNU32	u32APIType		= 1;
	IGNU32	u32WindowWidth	= 1024;
	IGNU32	u32WindowHeight	= 768;

	//
	// Create the demo instance and display window
	//
	HWND hWnd = CreateTestWindow( a_hInstance, u32WindowWidth, u32WindowHeight );	
	FRAME* pFrame = new DEMOFRAME( hWnd, u32APIType );
	if( NULL==pFrame )
		return 0;

	//
	// Loop the message processing
	//
	MSG msg; 
	memset( &msg, 0, sizeof(msg) );
    while( msg.message!=WM_QUIT )
	{
		if( !pFrame->Proceed( hWnd, &msg ) )
			break;
		if( PeekMessage( &msg, 
						 NULL, 
						 0U, 
						 0U, 
						 PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	//
	// Turn down the system ...
	//
	delete pFrame;
	return 0;
}
