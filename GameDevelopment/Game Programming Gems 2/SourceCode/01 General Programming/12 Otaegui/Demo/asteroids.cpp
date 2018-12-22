/************************************************************
 * Asteroids.cpp V0.00.001
 * 
 * (c) Copyright 2001 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/
 
// Run a simple asteroids demo

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <dinput.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#include "input.hpp"
#include "blitbuf.hpp"
#include "support.hpp"
#include "prim.hpp"

#include "fplog.h"

#include "stack.h"
#include "mainptah7.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

void
CargarGraficosAsteroid( void *data )
{
	blitbuf tempPCX;
	int i;
	const int bsize=30;
	blitbuf *launcher;

	fplog( "CargarGraficosAsteroid()\n" );

	launcher = (blitbuf *) data;

	// Cargo los graficos
	allocload_blitbufPCX( "launcher.lfb", &tempPCX );
	
	for( i=0; i<32; i++ )
	{
		alloc_blitbuf( &(launcher[i]), bsize, bsize );

		copy_blitbuf( &tempPCX, &(launcher[i]), (i%8)*bsize, (i/8)*bsize, (i%8)*bsize+bsize, (i/8*bsize+bsize), 0, 0 );
	}

	// Borro el temporal
	clear_blitbuf( &tempPCX );
}

void
RunAsteroids(void)
{
	char strTemp[512];
	int SsPosX=PhysicalWidth<<3, SsPosY=PhysicalHeight<<3, SsAngle=0, SsSpeedX=0, SsSpeedY=0;
	int k=0;
	int i=0;
	blitbuf launcher[32];
	int data=0;
	CallbackT Cback;

	// Push my initialization function
	Cback.func = (void *) &CargarGraficosAsteroid;
	Cback.data = (void *) launcher;

	gReloadSurfacesStack.Push( &Cback );

	// Load the graphics for the first time
	CargarGraficosAsteroid( launcher );

	// The "game" 
	while( k!=27 )
	{
		// Check input
		if( kbhit() )	
		{
			k=getch();

			switch( k )
			{
			case VK_LEFT:
				if( SsAngle > 0 )
					SsAngle--;
				else
					SsAngle=31;
				break;
			case VK_RIGHT:
				if( SsAngle < 31 )
					SsAngle++;
				else
					SsAngle=0;
				break;
			case VK_DOWN:
				SsSpeedX=SsSpeedX-(int)(8*sin(SsAngle*(3.14159/16)));
				SsSpeedY=SsSpeedY+(int)(8*cos(SsAngle*(3.14159/16)));
				break;
			case VK_UP:
				SsSpeedX=SsSpeedX+(int)(8*sin(SsAngle*(3.14159/16)));
				SsSpeedY=SsSpeedY-(int)(8*cos(SsAngle*(3.14159/16)));
				break;
			}
		}

		MouseEvent();

		// Updateo el mundo
		SsPosX = SsPosX+SsSpeedX;
		SsPosY = SsPosY+SsSpeedY;

		if( SsPosX<0 ) SsPosX = PhysicalWidth<<4;
		if( SsPosY<0 ) SsPosY = PhysicalHeight<<4;
		if( SsPosX>(PhysicalWidth<<4) ) SsPosX = 0;
		if( SsPosY>(PhysicalHeight<<4) ) SsPosY = 0;

		// Draw the Screen
		FillScreen( Make16(0,0,0) );

		sprintf( strTemp, "%4d %4d %4d %4d %4d", SsPosX, SsPosY, SsAngle, SsSpeedX, SsSpeedY );
		VGAputs( 0, 0, strTemp, Make16(255,255,255) );

		t_blitbuf( &launcher[SsAngle], (SsPosX>>4)-15, (SsPosY>>4)-15 );

		display_vscreen();
	}

	for( i=0; i<32; i++ )
		clear_blitbuf( &launcher[i] );

	// Retiro del stack mi fn de inicializacion
	gReloadSurfacesStack.Pop( &Cback );
}

