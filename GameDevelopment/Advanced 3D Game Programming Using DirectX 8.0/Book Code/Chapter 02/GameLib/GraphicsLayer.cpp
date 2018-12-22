/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GraphicsLayer.cpp
 *    Desc: Definition of a layer abstracting Direct3D
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/


#include <algorithm>

#include "stdafx.h"
#include "GraphicsLayer.h"
#include "Application.h"
#include "GameErrors.h"
#include "DxHelper.h"
#include "Window.h"

using namespace std;


/**
 * This is the static member variable that points to the one
 * (and only one) graphics layer allowed in each application.
 */
cGraphicsLayer* cGraphicsLayer::m_pGlobalGLayer = NULL;

cGraphicsLayer::cGraphicsLayer( HWND hWnd )
{
	if( m_pGlobalGLayer )
		throw cGameError( 
			"cGraphicsLayer object already instantiated\n");
	m_pGlobalGLayer = this;

	m_hWnd = hWnd ;

	m_pD3D			 = 0;
	m_pDevice		 = 0;

	m_pBackSurf		 = 0;
}


void cGraphicsLayer::DestroyAll()
{

    SafeRelease( m_pBackSurf );
	SafeRelease( m_pDevice );

	SafeRelease( m_pD3D );

	/**
	 * Prevent any further access to the graphics class
	 */
	m_pGlobalGLayer = NULL;
}

cGraphicsLayer::~cGraphicsLayer()
{
	DestroyAll();
}


void cGraphicsLayer::Flip()
{

	HRESULT r = 0;

	// Make sure the device has been created.
	assert( m_pDevice );
	
	// Blit the back buffer to the primary surface
	r = m_pDevice->Present( NULL, NULL, NULL, NULL );
	if( FAILED( r ) )
	{
		OutputDebugString( "Flipping Failed!\n" );
	}

}


void cGraphicsLayer::InitD3DFullScreen( 
	GUID* pGuid, int width, int height, int bpp )
{

	HRESULT r = 0;

	// Create the Direct3D object
	m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
	if( !m_pD3D )
	{
		throw cGameError( "Could not create IDirect3D8" );
	}

	// Structure to hold the creation parameters for the device
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );

	// The width and height for the initial back buffer
	d3dpp.BackBufferWidth			= width;
	d3dpp.BackBufferHeight			= height;
	
	// Set the flags for the bit depth - only supports 16, 24, and 32 bit formats
	if( bpp == 16 )
		d3dpp.BackBufferFormat		= D3DFMT_R5G6B5;
	else if( bpp == 24 )
		d3dpp.BackBufferFormat		= D3DFMT_X8R8G8B8;
	else if( bpp == 32 )
		d3dpp.BackBufferFormat		= D3DFMT_A8R8G8B8;
	else
	{
		OutputDebugString( "Invalid surface format - defaulting to 32bit" );
		d3dpp.BackBufferFormat		= D3DFMT_A8R8G8B8;
	}

	// Only have one back buffer associated with the primary surface
	d3dpp.BackBufferCount			= 1;
	// No multisampling
	d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;
	// Copy the back buffer to the primary surface normally
	d3dpp.SwapEffect				= D3DSWAPEFFECT_COPY;
	// The handle to the window to render in to
	d3dpp.hDeviceWindow				= m_hWnd;
	// Fullscreen operation
	d3dpp.Windowed					= FALSE;
	// Let Direct3D look after the depth buffer
	d3dpp.EnableAutoDepthStencil	= TRUE;
	// Set the depth buffer depth to 16 bits
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	// Use the default refresh rate
	d3dpp.FullScreen_RefreshRateInHz= D3DPRESENT_RATE_DEFAULT;
	// Update the screen as soon as possible (dont wait for vsync)
	d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	// Allow the back buffer to be locked
	d3dpp.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	
	// Create the device using hardware acceleration if available
	r = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
							D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
							&d3dpp, &m_pDevice );
	if( FAILED( r ) )
	{
		throw cGameError( "Could not create IDirect3DDevice8" );
	}

	// Keep a copy of the screen dimensions
	m_rcScreenRect.left = m_rcScreenRect.top = 0;
	m_rcScreenRect.right = width;
	m_rcScreenRect.bottom = height;

	// Get a copy of the pointer to the back buffer
	m_pDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackSurf );


}


void cGraphicsLayer::DrawTextString( int x, int y, 
							DWORD color, const char * str )
{

	HRESULT r = 0;
	
	if( !m_pBackSurf )
		return;

	// Get a handle for the font to use
	HFONT hFont = (HFONT)GetStockObject( SYSTEM_FONT );	
	
	LPD3DXFONT pFont = 0;
	// Create the D3DX Font
	r = D3DXCreateFont( m_pDevice, hFont, &pFont );
	if( FAILED( r ) )
		return;

	// Rectangle where the text will be located
	RECT TextRect = { x, y, 0, 0 };

	// Inform font it is about to be used
	pFont->Begin();

	// Calculate the rectangle the text will occupy
	pFont->DrawText( str, -1, &TextRect, DT_CALCRECT, 0 );

	// Output the text, left aligned
	pFont->DrawText( str, -1, &TextRect, DT_LEFT, color );

	// Finish up drawing
	pFont->End();

	// Release the font
	pFont->Release();

}



void cGraphicsLayer::Create(
	HWND hWnd,
	short width, short height,
	GUID* pGuid )
{
	new cGraphicsLayer( hWnd ); // construct the object.

	// Init Direct3D and the device for fullscreen operation
	Graphics()->InitD3DFullScreen( pGuid, width, height, 32 );
}
