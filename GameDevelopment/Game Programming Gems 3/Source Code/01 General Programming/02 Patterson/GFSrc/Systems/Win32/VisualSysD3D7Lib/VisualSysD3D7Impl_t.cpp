/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSysD3D7Impl_t.cpp: implementation of the VisualSysD3D7Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisualSysD3D7Impl_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/Win32/WindowSysWin32Impl_t.h"
#include "GFInclude/Objects/Box2D_t.h"
#include "GFInclude/Objects/TextBox2D_t.h"
#include "GFInclude/Objects/MenuBox2D_t.h"
#include "D3D7Font_t.h"

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "d3dx.lib")
#pragma comment(lib, "dxguid.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VisualSysD3D7Impl_t::VisualSysD3D7Impl_t()
{
	m_pSystems = 0;
	m_pDD = NULL;             // IDirectDraw7 Interface
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
    m_pddsFrontBuffer = NULL;    // The primary surface
    m_pddsBackBuffer = NULL;     // The backbuffer surface
    m_pddsZBuffer = NULL;        // The zbuffer surface
    m_pDeviceInfo = NULL;
    m_fnConfirmDevice = NULL;
	m_backColor = D3DRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	m_pFont = NULL;
}

VisualSysD3D7Impl_t::~VisualSysD3D7Impl_t()
{
	Shutdown();
}

bool VisualSysD3D7Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	m_bWindowed = TRUE;
	m_bZBuffer = TRUE;
    m_pFont = new D3D7Font_t( _T("Arial"), 12, D3D7FONT_BOLD );

	// get the window sys
	WindowSys_t *pWS = m_pSystems->GetWindowSys();
	int nWidth;
	int nHeight;
	pWS->GetWindowInfo( nWidth, nHeight );

	if( !m_bWindowed )
	{
		//get rid of the mouse cursor
		ShowCursor(FALSE);
	}

	// we know this is the Win32 implementation
	WindowSysWin32Impl_t *pWin = (WindowSysWin32Impl_t *)pWS;
	HRESULT hr = CreateDevice(pWin->m_hWnd, nWidth, nHeight, 16);
	if(FAILED(hr))
	{
		return false;
	}

    // Initialize device-dependent objects
    if( InitDeviceObjects() )
	{
		// Restore device-dependent objects
        if( RestoreDeviceObjects() )
		{
            return true;
		} else {
			return false;
		}
	}
	return false;
}

void VisualSysD3D7Impl_t::Shutdown()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	if( m_pFont )
	{
		delete m_pFont;
		m_pFont = NULL;
	}

	D3DEnum_FreeResources();

    LONG nDD  = 0L; // Number of outstanding DDraw references
    LONG nD3D = 0L; // Number of outstanding D3DDevice references

    if( m_pDD )
    {
        m_pDD->SetCooperativeLevel( m_hWnd, DDSCL_NORMAL );
    }

    // Do a safe check for releasing the D3DDEVICE. RefCount must be zero.
    if( m_pD3DDevice )
	{
        if( 0 < ( nD3D = m_pD3DDevice->Release() ) )
		{
            //DEBUG_MSG( _T("Error: D3DDevice object is still referenced!") );
		}
	}
    m_pD3DDevice = NULL;

    if( m_pddsBackBuffer )
	{
		m_pddsBackBuffer->Release();
		m_pddsBackBuffer = NULL;
	}
#if 0
    if( m_pddsBackBufferLeft )
	{
		m_pddsBackBufferLeft->Release();
		m_pddsBackBufferLeft = NULL;
	}
#endif
    if( m_pddsZBuffer )
	{
		m_pddsZBuffer->Release();
		m_pddsZBuffer = NULL;
	}
    if( m_pddsFrontBuffer )
	{
		m_pddsFrontBuffer->Release();
		m_pddsFrontBuffer = NULL;
	}
	if( m_pD3D )
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

    if( m_pDD )
    {
        // Do a safe check for releasing DDRAW. RefCount must be zero.
        if( 0 < ( nDD = m_pDD->Release() ) )
		{
            //DEBUG_MSG( _T("Error: DDraw object is still referenced!") );
		}
    }
    m_pDD = NULL;

    // Return successful, unless there are outstanding DD or D3DDevice refs.
    //return ( nDD==0 && nD3D==0 ) ? S_OK : D3DFWERR_NONZEROREFCOUNT;

	if( !m_bWindowed )
	{
		//restore cursor
		ShowCursor(TRUE);
	}
}

bool VisualSysD3D7Impl_t::InitDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->InitDeviceObjects( m_pD3DDevice );
	}
	return true;
}

bool VisualSysD3D7Impl_t::RestoreDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->RestoreDeviceObjects();
	}
	return true;
}

bool VisualSysD3D7Impl_t::InvalidateDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->InvalidateDeviceObjects();
	}
	return true;
}

bool VisualSysD3D7Impl_t::DeleteDeviceObjects()
{
	if( m_pFont )
	{
		m_pFont->DeleteDeviceObjects();
	}
	return true;
}

bool VisualSysD3D7Impl_t::BeginRender()
{
	m_pD3DDevice->Clear(0, 
					   NULL,
					   D3DCLEAR_TARGET |
					   D3DCLEAR_ZBUFFER,
					   m_backColor, //for now clear to blue
					   1.0f,
					   0);

	return( m_pD3DDevice->BeginScene() == S_OK );
}

void VisualSysD3D7Impl_t::EndRender()
{
	m_pFont->DrawText( 2, 2, 0xffffffff, "D3D7 Driver" );

	m_pD3DDevice->EndScene();
	ShowFrame();
}

//-----------------------------------------------------------------------------
// Name: Move()
// Desc: Moves the screen rect for windowed renderers
//-----------------------------------------------------------------------------
void VisualSysD3D7Impl_t::WindowMove( int nX, int nY )
{
	if( !m_bWindowed )
	{
		return;
	}

    SetRect( &m_rcScreenRect, nX, nY, nX + m_dwRenderWidth, nY + m_dwRenderHeight );
}

HRESULT VisualSysD3D7Impl_t::CreateDevice(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth)
{
	HRESULT hr;

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
	m_dwDepth = dwDepth;
    m_dwRenderWidth = m_dwWidth;
    m_dwRenderHeight = m_dwHeight;
	m_hWnd = hWnd;

	// Init the enumeration globals
	D3DEnum_InitGlobals();

    // Enumerate available D3D devices. The callback is used so the app can
    // confirm/reject each enumerated device depending on its capabilities.
    if( FAILED( hr = D3DEnum_EnumerateDevices( m_fnConfirmDevice ) ) )
    {
        //DisplayFrameworkError( hr, MSGERR_APPMUSTEXIT );
        return hr;
    }

    // Select a device. Ask for a hardware device that renders in a window.
    if( FAILED( hr = D3DEnum_SelectDefaultDevice( &m_pDeviceInfo ) ) )
    {
        //DisplayFrameworkError( hr, MSGERR_APPMUSTEXIT );
        return hr;
    }

	return CreateEnvironment( m_pDeviceInfo->pDriverGUID, m_pDeviceInfo->pDeviceGUID, &m_pDeviceInfo->ddsdFullscreenMode );
}

HRESULT VisualSysD3D7Impl_t::CreateEnvironment( GUID* pDriverGUID, GUID* pDeviceGUID, DDSURFACEDESC2* pMode )
{
    HRESULT hr;

    // Select the default memory type, for whether the device is HW or SW
    if( IsEqualIID( *pDeviceGUID, IID_IDirect3DHALDevice) )
        m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;
    else if( IsEqualIID( *pDeviceGUID, IID_IDirect3DTnLHalDevice) )
        m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;
    else
        m_dwDeviceMemType = DDSCAPS_SYSTEMMEMORY;

    // Create the DDraw object
    hr = CreateDirectDraw( pDriverGUID );
    if( FAILED( hr ) )
        return hr;

    // Create the front and back buffers, and attach a clipper
	if( !m_bWindowed )
	{
        hr = CreateFullscreenBuffers( pMode );
	} else {
        hr = CreateWindowedBuffers();
	}
    if( FAILED( hr ) )
        return hr;

    // Create the Direct3D object and the Direct3DDevice object
    hr = CreateDirect3D( pDeviceGUID );
    if( FAILED( hr ) )
        return hr;

    // Create and attach the zbuffer
    if( m_bZBuffer )
        hr = CreateZBuffer( pDeviceGUID );
    if( FAILED( hr ) )
        return hr;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateDirectDraw()
// Desc: Create the DirectDraw interface
//-----------------------------------------------------------------------------
HRESULT VisualSysD3D7Impl_t::CreateDirectDraw( GUID* pDriverGUID )
{
    // Create the DirectDraw interface, and query for the DD7 interface
    if( FAILED( DirectDrawCreateEx( pDriverGUID, (VOID**)&m_pDD,
                                    IID_IDirectDraw7, NULL ) ) )
    {
        //DEBUG_MSG( _T("Could not create DirectDraw") );
        //return D3DFWERR_NODIRECTDRAW;
		return E_FAIL;
    }

    // Set the Windows cooperative level
    DWORD dwCoopFlags = DDSCL_NORMAL;
	if( !m_bWindowed )
	{
        dwCoopFlags = DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN;
	}

    // By defualt, set the flag to allow D3D to optimize floating point calcs
    //if( 0L == ( dwFlags & D3DFW_NO_FPUSETUP ) )
    dwCoopFlags |= DDSCL_FPUSETUP;

    if( FAILED( m_pDD->SetCooperativeLevel( m_hWnd, dwCoopFlags ) ) )
    {
        //DEBUG_MSG( _T("Couldn't set coop level") );
        //return D3DFWERR_COULDNTSETCOOPLEVEL;
		return E_FAIL;
    }

    // Check that we are NOT in a palettized display. That case will fail,
    // since the framework doesn't use palettes.
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    m_pDD->GetDisplayMode( &ddsd );
    if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
	{
        //return D3DFWERR_INVALIDMODE;
		return E_FAIL;
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateFullscreenBuffers()
// Desc: Creates the primary and (optional) backbuffer for rendering.
//       Windowed mode and fullscreen mode are handled differently.
//-----------------------------------------------------------------------------
HRESULT VisualSysD3D7Impl_t::CreateFullscreenBuffers( DDSURFACEDESC2* pddsd )
{
    HRESULT hr;

    // Get the dimensions of the screen bounds
    // Store the rectangle which contains the renderer
    SetRect( &m_rcScreenRect, 0, 0, pddsd->dwWidth, pddsd->dwHeight );
    m_dwRenderWidth  = m_rcScreenRect.right  - m_rcScreenRect.left;
    m_dwRenderHeight = m_rcScreenRect.bottom - m_rcScreenRect.top;

    // Set the display mode to the requested dimensions. Check for
    // 320x200x8 modes, and set flag to avoid using ModeX
    DWORD dwModeFlags = 0;

    if( (320==m_dwRenderWidth) && (200==m_dwRenderHeight) &&
        (8==pddsd->ddpfPixelFormat.dwRGBBitCount) )
        dwModeFlags |= DDSDM_STANDARDVGAMODE;

    if( FAILED( m_pDD->SetDisplayMode( m_dwRenderWidth, m_dwRenderHeight,
                                pddsd->ddpfPixelFormat.dwRGBBitCount,
                                pddsd->dwRefreshRate, dwModeFlags ) ) )
    {
        //DEBUG_MSG( _T("Can't set display mode") );
        //return D3DFWERR_BADDISPLAYMODE;
		return E_FAIL;
    }

    // Setup to create the primary surface w/backbuffer
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize            = sizeof(ddsd);
    ddsd.dwFlags           = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE |
                             DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;

#if 0
    // Support for stereoscopic viewing
    if( m_bIsStereo )
    {
        ddsd.ddsCaps.dwCaps  |= DDSCAPS_VIDEOMEMORY;
        ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_STEREOSURFACELEFT;
    }
#endif

    // Create the primary surface
    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer, NULL ) ) )
    {
        //DEBUG_MSG( _T("Error: Can't create primary surface") );
        //if( hr != DDERR_OUTOFVIDEOMEMORY )
        //    return D3DFWERR_NOPRIMARY;
        //DEBUG_MSG( _T("Error: Out of video memory") );
        //return DDERR_OUTOFVIDEOMEMORY;
		return E_FAIL;
    }

    // Get the backbuffer, which was created along with the primary.
    DDSCAPS2 ddscaps = { DDSCAPS_BACKBUFFER, 0, 0, 0 };
    if( FAILED( hr = m_pddsFrontBuffer->GetAttachedSurface( &ddscaps,
                                                &m_pddsBackBuffer ) ) )
    {
        //DEBUG_ERR( hr, _T("Error: Can't get the backbuffer") );
        //return D3DFWERR_NOBACKBUFFER;
		return E_FAIL;
    }

    // Increment the backbuffer count (for consistency with windowed mode)
    m_pddsBackBuffer->AddRef();

#if 0
    // Support for stereoscopic viewing
    if( m_bIsStereo )
    {
        // Get the left backbuffer, which was created along with the primary.
        DDSCAPS2 ddscaps = { 0, DDSCAPS2_STEREOSURFACELEFT, 0, 0 };
        if( FAILED( hr = m_pddsBackBuffer->GetAttachedSurface( &ddscaps,
                                                    &m_pddsBackBufferLeft ) ) )
        {
            DEBUG_ERR( hr, _T("Error: Can't get the left backbuffer") );
            return D3DFWERR_NOBACKBUFFER;
        }
        m_pddsBackBufferLeft->AddRef();
    }
#endif

	//FILE *fplog = fopen(FNAME_LOGFILE,"a");

	ZeroMemory(&m_ddpfBackBufferPixelFormat, sizeof(DDPIXELFORMAT));
	m_ddpfBackBufferPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	hr = m_pddsBackBuffer->GetPixelFormat(&m_ddpfBackBufferPixelFormat);

	//if(hr == DD_OK)
	//{		
		//fprintf( fplog, "Backbuffer Pixelformat RGB bits  = : ");
		//fprintf( fplog, "%d\n",m_ddpfBackBufferPixelFormat.dwRGBBitCount);

		//fprintf( fplog, "Backbuffer Pixelformat RGB masks = : ");
		//fprintf( fplog, "%d %d %d\n\n",
		//m_ddpfBackBufferPixelFormat.dwRBitMask,
		//m_ddpfBackBufferPixelFormat.dwGBitMask,
		//m_ddpfBackBufferPixelFormat.dwBBitMask);
	//} else {
		//fprintf( fplog, "Check on backbuffer pixel format FAILED ");
	//}

	hr = m_pddsBackBuffer->GetSurfaceDesc( &ddsd );
	
	//fclose(fplog);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateWindowedBuffers()
// Desc: Creates the primary and (optional) backbuffer for rendering.
//       Windowed mode and fullscreen mode are handled differently.
//-----------------------------------------------------------------------------
HRESULT VisualSysD3D7Impl_t::CreateWindowedBuffers()
{
    HRESULT hr;

    // Get the dimensions of the viewport and screen bounds
    GetClientRect( m_hWnd, &m_rcScreenRect );
    ClientToScreen( m_hWnd, (POINT*)&m_rcScreenRect.left );
    ClientToScreen( m_hWnd, (POINT*)&m_rcScreenRect.right );
    DWORD m_dwRenderWidth  = m_rcScreenRect.right  - m_rcScreenRect.left;
    DWORD m_dwRenderHeight = m_rcScreenRect.bottom - m_rcScreenRect.top;

    // Create the primary surface
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer, NULL ) ) )
    {
        //DEBUG_MSG( _T("Error: Can't create primary surface") );
        //if( hr != DDERR_OUTOFVIDEOMEMORY )
        //    return D3DFWERR_NOPRIMARY;
        //DEBUG_MSG( _T("Error: Out of video memory") );
        //return DDERR_OUTOFVIDEOMEMORY;
		return E_FAIL;
    }

    // If in windowed-mode, create a clipper object
    LPDIRECTDRAWCLIPPER pcClipper;
    if( FAILED( hr = m_pDD->CreateClipper( 0, &pcClipper, NULL ) ) )
    {
        //DEBUG_MSG( _T("Error: Couldn't create clipper") );
        //return D3DFWERR_NOCLIPPER;
		return E_FAIL;
    }

    // Associate the clipper with the window
    pcClipper->SetHWnd( 0, m_hWnd );
    m_pddsFrontBuffer->SetClipper( pcClipper );
	if( pcClipper )
	{
		pcClipper->Release();
	}

    // Create a backbuffer
    ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.dwWidth        = m_dwRenderWidth;
    ddsd.dwHeight       = m_dwRenderHeight;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsBackBuffer, NULL ) ) )
    {
        //DEBUG_ERR( hr, _T("Error: Couldn't create the backbuffer") );
        //if( hr != DDERR_OUTOFVIDEOMEMORY )
        //    return D3DFWERR_NOBACKBUFFER;
        //DEBUG_MSG( _T("Error: Out of video memory") );
        //return DDERR_OUTOFVIDEOMEMORY;
		return E_FAIL;
    }

	ZeroMemory(&m_ddpfBackBufferPixelFormat, sizeof(DDPIXELFORMAT));
	m_ddpfBackBufferPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	hr = m_pddsBackBuffer->GetPixelFormat(&m_ddpfBackBufferPixelFormat);
	hr = m_pddsBackBuffer->GetSurfaceDesc( &ddsd );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumZBufferFormatsCallback()
// Desc: Simply returns the first matching enumerated z-buffer format
//-----------------------------------------------------------------------------
static HRESULT WINAPI EnumZBufferFormatsCallback( DDPIXELFORMAT* pddpf,
                                                  VOID* pContext )
{
    DDPIXELFORMAT* pddpfOut = (DDPIXELFORMAT*)pContext;

    if( pddpfOut->dwRGBBitCount == pddpf->dwRGBBitCount )
    {
        (*pddpfOut) = (*pddpf);
        return D3DENUMRET_CANCEL;
    }

    return D3DENUMRET_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateZBuffer()
// Desc: Internal function called by Create() to make and attach a zbuffer
//       to the renderer
//-----------------------------------------------------------------------------
HRESULT VisualSysD3D7Impl_t::CreateZBuffer( GUID* pDeviceGUID )
{
    HRESULT hr;

    // Check if the device supports z-bufferless hidden surface removal. If so,
    // we don't really need a z-buffer
    D3DDEVICEDESC7 ddDesc;
    m_pD3DDevice->GetCaps( &ddDesc );
    if( ddDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR )
        return S_OK;

    // Get z-buffer dimensions from the render target
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    m_pddsBackBuffer->GetSurfaceDesc( &ddsd );

    // Setup the surface desc for the z-buffer.
    ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | m_dwDeviceMemType;
    ddsd.ddpfPixelFormat.dwSize = 0;  // Tag the pixel format as unitialized

    // Get an appropiate pixel format from enumeration of the formats. On the
    // first pass, we look for a zbuffer dpeth which is equal to the frame
    // buffer depth (as some cards unfornately require this).
    m_pD3D->EnumZBufferFormats( *pDeviceGUID, EnumZBufferFormatsCallback,
                                (VOID*)&ddsd.ddpfPixelFormat );
    if( 0 == ddsd.ddpfPixelFormat.dwSize )
    {
        // Try again, just accepting any 16-bit zbuffer
        ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
        m_pD3D->EnumZBufferFormats( *pDeviceGUID, EnumZBufferFormatsCallback,
                                    (VOID*)&ddsd.ddpfPixelFormat );
            
        if( 0 == ddsd.ddpfPixelFormat.dwSize )
        {
            //DEBUG_MSG( _T("Device doesn't support requested zbuffer format") );
            //return D3DFWERR_NOZBUFFER;
			return E_FAIL;
        }
    }

    // Create and attach a z-buffer
    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsZBuffer, NULL ) ) )
    {
        //DEBUG_MSG( _T("Error: Couldn't create a ZBuffer surface") );
        //if( hr != DDERR_OUTOFVIDEOMEMORY )
        //    return D3DFWERR_NOZBUFFER;
        //DEBUG_MSG( _T("Error: Out of video memory") );
        //return DDERR_OUTOFVIDEOMEMORY;
		return E_FAIL;
    }

    if( FAILED( m_pddsBackBuffer->AddAttachedSurface( m_pddsZBuffer ) ) )
    {
        //DEBUG_MSG( _T("Error: Couldn't attach zbuffer to render surface") );
        //return D3DFWERR_NOZBUFFER;
		return E_FAIL;
    }

#if 0
    // For stereoscopic viewing, attach zbuffer to left surface as well
    if( m_bIsStereo )
    {
        if( FAILED( m_pddsBackBufferLeft->AddAttachedSurface( m_pddsZBuffer ) ) )
        {
            DEBUG_MSG( _T("Error: Couldn't attach zbuffer to left render surface") );
            return D3DFWERR_NOZBUFFER;
        }
    }
#endif

    // Finally, this call rebuilds internal structures
    if( FAILED( m_pD3DDevice->SetRenderTarget( m_pddsBackBuffer, 0L ) ) )
    {
        //DEBUG_MSG( _T("Error: SetRenderTarget() failed after attaching zbuffer!") );
        //return D3DFWERR_NOZBUFFER;
		return E_FAIL;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CreateDirect3D()
// Desc: Create the Direct3D interface
//-----------------------------------------------------------------------------
HRESULT VisualSysD3D7Impl_t::CreateDirect3D( GUID* pDeviceGUID )
{
    // Query DirectDraw for access to Direct3D
    if( FAILED( m_pDD->QueryInterface( IID_IDirect3D7, (VOID**)&m_pD3D ) ) )
    {
        //DEBUG_MSG( _T("Couldn't get the Direct3D interface") );
        //return D3DFWERR_NODIRECT3D;
		return E_FAIL;
    }

    // Create the device
    if( FAILED( m_pD3D->CreateDevice( *pDeviceGUID, m_pddsBackBuffer,
                                      &m_pD3DDevice) ) )
    {
        //DEBUG_MSG( _T("Couldn't create the D3DDevice") );
        //return D3DFWERR_NO3DDEVICE;
		return E_FAIL;
    }

    // Finally, set the viewport for the newly created device
    D3DVIEWPORT7 vp = { 0, 0, m_dwRenderWidth, m_dwRenderHeight, 0.0f, 1.0f };

    if( FAILED( m_pD3DDevice->SetViewport( &vp ) ) )
    {
        //DEBUG_MSG( _T("Error: Couldn't set current viewport to device") );
        //return D3DFWERR_NOVIEWPORT;
		return E_FAIL;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FlipToGDISurface()
// Desc: Puts the GDI surface in front of the primary, so that dialog
//       boxes and other windows drawing funcs may happen.
//-----------------------------------------------------------------------------
HRESULT VisualSysD3D7Impl_t::FlipToGDISurface( BOOL bDrawFrame )
{
    if( m_pDD && !m_bWindowed )
    {
        m_pDD->FlipToGDISurface();

        if( bDrawFrame )
        {
            DrawMenuBar( m_hWnd );
            RedrawWindow( m_hWnd, NULL, NULL, RDW_FRAME );
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ShowFrame()
// Desc: Show the frame on the primary surface, via a blt or a flip.
//-----------------------------------------------------------------------------
HRESULT VisualSysD3D7Impl_t::ShowFrame()
{
    if( NULL == m_pddsFrontBuffer )
	{
        //return D3DFWERR_NOTINITIALIZED;
		return E_FAIL;
	}

    if( !m_bWindowed )
    {
        // We are in fullscreen mode, so perform a flip.
        //if( m_bIsStereo )
		//{
        //    return m_pddsFrontBuffer->Flip( NULL, DDFLIP_WAIT | DDFLIP_STEREO );
		//} else {
            return m_pddsFrontBuffer->Flip( NULL, DDFLIP_WAIT );
		//}
    }
    else
    {
        // We are in windowed mode, so perform a blit.
        return m_pddsFrontBuffer->Blt( &m_rcScreenRect, m_pddsBackBuffer,
                                       NULL, DDBLT_WAIT, NULL );
    }
}

void VisualSysD3D7Impl_t::RenderBox( Box2D_t *pBox )
{
	m_pFont->DrawBox( pBox->m_fXPos, pBox->m_fYPos, pBox->m_fXSize, pBox->m_fYSize, pBox->m_nColor );
}

void VisualSysD3D7Impl_t::RenderTextBox( TextBox2D_t *pTextBox )
{
	if( pTextBox->m_bDrawBox )
	{
		m_pFont->DrawBox( pTextBox->m_fXPos, pTextBox->m_fYPos, pTextBox->m_fXSize, pTextBox->m_fYSize, pTextBox->m_nBoxColor );
	}
	if( pTextBox->m_bDrawText )
	{
		m_pFont->DrawText( pTextBox->m_fXPos, pTextBox->m_fYPos, pTextBox->m_nTextColor, pTextBox->m_Text.c_str() );
	}
}

void VisualSysD3D7Impl_t::RenderMenuBox( MenuBox2D_t *pMenuBox )
{
	int nChoice = pMenuBox->m_nChoice;
	int nCount = 0;
	TextBox2D_Vector_t::iterator iTBV;
	for( iTBV = pMenuBox->m_TextBoxVector.begin(); iTBV != pMenuBox->m_TextBoxVector.end(); ++iTBV )
	{
		TextBox2D_t &TB = *iTBV;
		if( TB.m_bDrawBox )
		{
			TB.m_nBoxColor = (nChoice == nCount) ? pMenuBox->m_nBoxHighlightColor : pMenuBox->m_nBoxNormalColor;
			m_pFont->DrawBox( pMenuBox->m_fXPos+TB.m_fXPos, pMenuBox->m_fYPos+TB.m_fYPos, TB.m_fXSize, TB.m_fYSize, TB.m_nBoxColor );
		}
		if( TB.m_bDrawText )
		{
			TB.m_nTextColor = (nChoice == nCount) ? pMenuBox->m_nTextHighlightColor : pMenuBox->m_nTextNormalColor;
			m_pFont->DrawText( pMenuBox->m_fXPos+TB.m_fXPos, pMenuBox->m_fYPos+TB.m_fYPos, TB.m_nTextColor, TB.m_Text.c_str() );
		}
		nCount++;
	}
}

void VisualSysD3D7Impl_t::RenderStats()
{
}

