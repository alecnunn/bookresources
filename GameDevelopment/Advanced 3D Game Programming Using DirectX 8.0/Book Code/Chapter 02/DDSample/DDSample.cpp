/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: DDSample.cpp
 *    Desc: Sample application for Direct3D
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include <string>
using namespace std;

class cD3DSampleApp : public cApplication
{

public:

	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );

	cD3DSampleApp() :
		cApplication()
	{
		m_title = string( "Direct3D Sample" );
	}
};

cApplication* CreateApplication()
{
	return new cD3DSampleApp();
}

void cD3DSampleApp::DoFrame( float timeDelta )
{

	HRESULT hr;
	if( !Graphics() ) return;

	// Clear the previous contents of the backbuffer
	Graphics()->GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
													D3DCOLOR_XRGB( 0,0,0), 1.0f, 0 );
	
	// Structure to hold information about the locked back buffer
	D3DLOCKED_RECT	LockedRect;

	// Get a local pointer to the back buffer
	LPDIRECT3DSURFACE8 pBackSurf = Graphics()->GetBackBuffer();

	// Lcck the back buffer
	pBackSurf->LockRect( &LockedRect, NULL, NULL );
	
	// Get a pointer to the back buffer
	DWORD* pData = (DWORD*)LockedRect.pBits;
	// Convert the pitch to work with 32 bit (4 byte) surfaces
	int Pitch32 = LockedRect.Pitch / 4;

	int x, y;		// Holds the location of the random pixel
	DWORD Color;	// Holds the color of the pixels and rectangles

	// ------------- PART 1: Draw 10,000 randomly colored pixels
	for( int i = 0 ; i < 10000 ; i++ )
	{
		// Get a random location for the pixel
		x = rand()%639;
		y = rand()%479;

		// Get a random color for the pixel
		Color = D3DCOLOR_XRGB( rand()%255, rand()%255, rand()%255 );
		
		// Set the pixel at x,y to the color
		pData[ Pitch32 * y + x ] = Color;
	}
	
	// ------------- PART 2: Draw 10 random rectangles
	
	RECT Rect;	// Structure to  hold the dimensions of the rectangles
	for( int j = 0 ; j < 10 ; j++ )
	{
		// Create a random sized rectangle
		SetRect( &Rect, rand()%639, rand()%479,
						rand()%639, rand()%479 );

		// Get a random rectangle color
		Color = D3DCOLOR_XRGB( rand()%255, rand()%255, rand()%255 );
		
		// Draw the rectangle (ie clear a rectangle to a color)
		Graphics()->GetDevice()->Clear( 1, (D3DRECT*)&Rect, D3DCLEAR_TARGET, 
													Color, 1.0f, 0 );
	}

	// Unlock the back surface. Very important to do this!
	pBackSurf->UnlockRect();
	pBackSurf = 0;

	// ------------- PART 3: Output text to the back surface
	// Tell Direct3D we are about to start rendering through Direct3D
	Graphics()->GetDevice()->BeginScene();
	
	// Output green text at a random location
	Graphics()->DrawTextString( rand()%640, rand()%480, 
			D3DCOLOR_XRGB( 0, 255, 0 ), "Advanced Direct3D 8.0" );

	// Tell Direct3D we are finished rendering
	Graphics()->GetDevice()->EndScene();

	// Present the back buffer to the primary surface to make it visible
	Graphics()->Flip();

}

