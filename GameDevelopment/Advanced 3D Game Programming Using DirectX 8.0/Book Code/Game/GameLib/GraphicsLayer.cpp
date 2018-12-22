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
	for( int i = 0 ; i < 8 ; i++ )
	{
		m_pDevice->SetTexture( i, NULL );
	}

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
	int Ordinal, DWORD flags, int width, int height, int bpp )
{

	HRESULT r = 0;

	// Create the Direct3D interface
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
		d3dpp.BackBufferFormat		= D3DFMT_R8G8B8;
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
	
	// If a depth buffer was requested
	if( flags & (GLF_ZBUFFER|GLF_HIRESZBUFFER) )
	{
		// Tell Direct3D we want a depth buffer
		d3dpp.EnableAutoDepthStencil	= TRUE;

		
		if( flags & (GLF_HIRESZBUFFER) )
		{
			
			if( flags & (GLF_STENCIL) )
				// 24 bit depth buffer and 8 bit stencil
				d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
			else
				// 32 bit depth buffer and no stencil
				d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
		}
		else
		{
			if( flags & (GLF_STENCIL) )
				// 15 bit depth buffer and 1 bit stencil
				d3dpp.AutoDepthStencilFormat = D3DFMT_D15S1;
			else
				// 16 bit depth buffer and no stencil
				d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		}
		
	}
	else
	{
		// No depth buffer or stencil
		d3dpp.EnableAutoDepthStencil = FALSE;	
	}
	
	// Use the default refresh rate
	d3dpp.FullScreen_RefreshRateInHz= D3DPRESENT_RATE_DEFAULT;
	// Update the screen as soon as possible (dont wait for vsync)
	d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	// Allow the back buffer to be locked
	d3dpp.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	
	// Hardware device by default
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

	if( flags & (GLF_FORCEHARDWARE) )
		DeviceType = D3DDEVTYPE_HAL;
	else if( flags & (GLF_FORCESOFTWARE) )
		DeviceType = D3DDEVTYPE_SW;
	else if( flags & (GLF_FORCEREFERENCE) )
		DeviceType = D3DDEVTYPE_REF;
	
	// Create the device using hardware acceleration if available
	r = m_pD3D->CreateDevice( Ordinal, DeviceType, m_hWnd,
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
	m_pDevice->GetBackBuffer( Ordinal, D3DBACKBUFFER_TYPE_MONO, &m_pBackSurf );

	// Setup the viewport
	MakeViewport();

	// Build the projection matrix
	MakeProjectionMatrix();

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
	ULONG flags,
	GUID* pGuid )
{
	new cGraphicsLayer( hWnd ); // construct the object.

	int depth = (flags & GLF_FORCE16BIT) ? 16 : 32;
	
	// Init Direct3D and the device for fullscreen operation
	Graphics()->InitD3DFullScreen( D3DADAPTER_DEFAULT, GLF_ZBUFFER, width, height, depth );
}

void cGraphicsLayer::SetProjectionData( 
	float inFov, 
	float inNear, 
	float inFar )
{
	m_fov = inFov;
	m_far = inFar;
	m_near = inNear;
}

void cGraphicsLayer::GetProjectionData( 
	float* inFov, 
	float* inNear, 
	float* inFar )
{
	if( inFov )
		*inFov = m_fov;
	if( inFar )
		*inFar = m_far;
	if( inNear )
		*inNear = m_near;
}

eResult cGraphicsLayer::MakeProjectionMatrix()
{
	D3DMATRIX mat;
	
	DWORD width, height;
	width = m_rcScreenRect.right;
	height = m_rcScreenRect.bottom;

	float fAspect = ((float)height) / width;

    if( fabs(m_far-m_near) < 0.01f )
        return resFailed;
    if( fabs(sin(m_fov/2)) < 0.01f )
        return resFailed;

	float w = fAspect * (float)( cos(m_fov/2)/sin(m_fov/2) );
	float h =   1.0f  * (float)( cos(m_fov/2)/sin(m_fov/2) );
    float Q = m_far / ( m_far - m_near );

    ZeroMemory( &mat, sizeof(D3DMATRIX) );
    mat._11 = w;
    mat._22 = h;
    mat._33 = Q;
    mat._34 = 1.0f;
    mat._43 = -Q*m_near;

	m_pDevice->SetTransform( D3DTS_PROJECTION, &mat );

    return resAllGood;
}

void cGraphicsLayer::GetViewMatrix( matrix4* pMat )
{
	// Pass the call to the Direct3D Device
	m_pDevice->GetTransform( 
		D3DTS_VIEW, 
		(D3DMATRIX*)pMat );
}

void cGraphicsLayer::SetViewMatrix( const matrix4& mat )
{
	// Pass the call to the Direct3D Device
	m_pDevice->SetTransform( 
		D3DTS_VIEW, 
		(D3DMATRIX*)&mat );
}

void cGraphicsLayer::GetProjectionMatrix( matrix4* pMat )
{
	// Pass the call to the Direct3D Device
	m_pDevice->GetTransform( 
		D3DTS_PROJECTION, 
		(D3DMATRIX*)pMat );
}

void cGraphicsLayer::SetProjectionMatrix( const matrix4& mat )
{
	// Pass the call to the Direct3D Device
	m_pDevice->SetTransform( 
		D3DTS_PROJECTION, 
		(D3DMATRIX*)&mat );
}

void cGraphicsLayer::SetWorldMatrix( const matrix4& mat )
{
	// Pass the call to the Direct3D Device
	m_pDevice->SetTransform( 
		D3DTS_WORLD, 
		(D3DMATRIX*)&mat );
}

void cGraphicsLayer::GetWorldMatrix( matrix4* pMat )
{
	// Pass the call to the Direct3D Device
	m_pDevice->GetTransform( 
		D3DTS_WORLD, 
		(D3DMATRIX*)pMat );
}

void cGraphicsLayer::Clear( 
	bool bClearFrame, 
	bool bClearZ, 
	DWORD frameColor, 
	float zValue )
{
	if( !m_pBackSurf )
		return;

	DWORD flags = bClearFrame ? D3DCLEAR_TARGET : 0L;
	flags |= bClearZ? D3DCLEAR_ZBUFFER : 0L;

	HRESULT hr;
	hr = m_pDevice->Clear( 0, 0, flags, frameColor, zValue, 0);

	if( FAILED( hr ))
	{
		DP0("[cGraphicsLayer::Clear]: Clear failed\n");
	}
}

void cGraphicsLayer::BeginScene()
{
	// Pass the call to the Direct3D Device
	m_pDevice->BeginScene();
}

void cGraphicsLayer::EndScene()
{
	// Pass the call to the Direct3D Device
	m_pDevice->EndScene();
}

void cGraphicsLayer::MakeViewport()
{
	HRESULT hr;
	if( !m_pDevice )
	{
		DP("[cGraphicsLayer::MakeViewport]: no device\n");
		return;
	}

    DWORD dwRenderWidth  = m_rcScreenRect.right;
    DWORD dwRenderHeight = m_rcScreenRect.bottom;
    D3DVIEWPORT8 vp = { 0, 0, dwRenderWidth, dwRenderHeight, 0.0f, 1.0f };

    hr = m_pDevice->SetViewport( &vp );
    if( FAILED( hr ) )
        throw cGameError("viewport setting failed.");
}

