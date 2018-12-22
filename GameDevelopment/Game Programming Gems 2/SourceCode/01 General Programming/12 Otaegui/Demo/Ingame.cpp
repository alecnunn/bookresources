/************************************************************
 * Ingame.cpp - Ingame Functions V2.16.001
 * 
 * (c) Copyright 2001 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <dinput.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>

#include "input.hpp"
#include "blitbuf.hpp"
#include "support.hpp"
#include "prim.hpp"
#include "mainptah7.h"
#include "letras.hpp"

#include "ingame.h"
#include "asteroids.h"
#include "main.h"

/////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

//
// This function is called to uninitialize the game
//
void
unInitGame( void )
{
	// Close DirectInput
    DITerm();

	// Close the video system
	unInitVideoSystem();
}


//
// This function is called if the initialization function fails
//
BOOL
initFailed( HWND hwnd, char *msg )
{
	unInitGame();

	SetCursor( NULL );
	
	if( msg==NULL )
		MessageBox( hwnd, "Error on initialization.", TITLE, MB_OK );
	else
		MessageBox( hwnd, msg, TITLE, MB_OK );

    return FALSE;
}

// Load standard typography
void
InitLetras( void *data )
{
	char strTemp[512];

	fplog( "InitLetras()\n" );

	// Tipografias estandar
	sprintf( strTemp, "letras6.lfb" );
	LoadLetras( fuentestandard, strTemp);
}

//
// This function is called to initialize the game
//
BOOL
InitGame( void )
{
	CallbackT Cback;

	PhysicalWidth=640;
	PhysicalHeight=480;

	// Init the video system
	initVideoSystem(PhysicalWidth, PhysicalHeight, PhysicalBPP);

	// Start DirectInput
    if (!DIInit(hwnd, g_hInstance))
    {
		return initFailed(hwnd, "DirectInput initialization failed. Aborting." );
	}

	// Register and Load Fonts
    bInicio=FALSE;

	Cback.func = (void *) &InitLetras;
	Cback.data = NULL;

	gReloadSurfacesStack.Push( &Cback );

	InitLetras( NULL );

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

// This will draw lines until ESC is hit
void
FrenesiLineal( void )
{
	int x=0;
	int k=0;

	// The loop
	while( k!=27 )
	{
		// Check inputs
		if( kbhit() )	
		{
			k=getch();

			// Additional input processing
		}

		// Clear Screen
		FillScreen( Make16(0,0,0) );

		// Draw random lines
		for( x=0; x<100; x++ )
		{
			Line( rand()%PhysicalWidth, rand()%PhysicalHeight, rand()%PhysicalWidth, rand()%PhysicalHeight, Make16(rand()%256, rand()%256, rand()%256) );
		}

		// Render
		display_vscreen();
	}
}

// This will load the required fonts for TextScroller()
void
LoadTextScrollerFont( void *data )
{
	FUENTE *MyFont;

	MyFont = (FUENTE *) data;

	LoadLetras( *MyFont, "verde.lfb" );
}

// This will scroll a text in big font
void
TextScroller( void )
{
	int x=0;
	int k=0;
	FUENTE MyFont;
	CallbackT Cback;

	// Initialize the required font surfaces
	LoadTextScrollerFont( (void *) &MyFont );

	// Push into reload stack
	Cback.func = (void *) &LoadTextScrollerFont;
	Cback.data = (void *) &MyFont;
	gReloadSurfacesStack.Push( &Cback );

	// The loop
	while( k!=27 )
	{
		// Check inputs
		if( kbhit() )	
		{
			k=getch();

			// Additional input processing
		}

		// Clear Screen
		FillScreen( Make16(0,0,0) );

		// Draw text
		VGAPutStringCenter( 0, MyFont, "GPG2", PhysicalHeight-x, Make16( 255,255,255 ) );
		VGAPutStringCenter( 0, MyFont, "A linear programming model", 2*MyFont.height+PhysicalHeight-x, Make16( 255,255,255 ) );
		VGAPutStringCenter( 0, MyFont, "for Windows-based games", 3*MyFont.height+PhysicalHeight-x, Make16( 255,255,255 ) );
		VGAPutStringCenter( 0, MyFont, "by Javier F. Otaegui", 5*MyFont.height+PhysicalHeight-x, Make16( 255,255,255 ) );
		VGAPutStringCenter( 0, MyFont, "javier@sabarasa.com", 6*MyFont.height+PhysicalHeight-x, Make16( 255,255,255 ) );

		// Render
		display_vscreen();

		x++;
		if( x>(PhysicalHeight+7*MyFont.height) )
			x=0;
	}

	// Remember to Pop
	gReloadSurfacesStack.Pop( &Cback );
}

// Simple splash function
void
Splash( void )
{
	TextScroller();
	FrenesiLineal();
}

void
RunGame( void )
{
	fplog( "ini RunGame()\n" );

	/////////////////////////////////////////////////////////////////////////////
	// INIT MY GAME
	/////////////////////////////////////////////////////////////////////////////
	if(!InitGame())
		return;

	//////////////////////////////////////////////////////////////////////////////
	// GAME EXECUTION
	//////////////////////////////////////////////////////////////////////////////
	Splash();
	RunAsteroids();
	Splash();

	//////////////////////////////////////////////////////////////////////////////
	// CLOSE EVERYTHING
	//////////////////////////////////////////////////////////////////////////////
	unInitGame();

	fplog( "fin RunGame()\n" );
}