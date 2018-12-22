/*********************************************************************NVMH2****
Path:  D:\Dev\devrel\Nv_sdk_4\Dx8\CommonSrc
File:  D3D8Manager.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:

You probably want to run these in 32-bit mode.  I don't test them in 16-bit.

For some cases, a depth buffer is not created.  There's no special reason for
this, and it could be put in easily.


******************************************************************************/



#include "D3D8Manager.h"
#include <assert.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

D3D8Manager::D3D8Manager()
{

}

D3D8Manager::~D3D8Manager()
{
	UnInitD3D();
}


///////////////////////////////////////////////////////////////////////

void	D3D8Manager::UnInitD3D()
{
	if( m_pd3dDevice != NULL) 
	{
		SAFE_RELEASE( m_pd3dDevice );
		// m_pd3dDevice->Release();
		m_pd3dDevice = NULL;
	}

	if( m_pD3D != NULL)
	{
		SAFE_RELEASE( m_pD3D );
		m_pD3D = NULL;
		//        m_pD3D->Release();
	}

}


//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes DX8 Direct3D
//   From the microsoft DX8 tutorial
//-----------------------------------------------------------------------------
HRESULT D3D8Manager::InitD3D( HWND hWnd )
{
    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( m_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
	{
		assert( false );
        return E_FAIL;
	}

    // Get the current desktop display mode
    D3DDISPLAYMODE d3ddm;
    if( FAILED( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		assert( false );
        return E_FAIL;
	}


    // Set up the structure used to create the D3DDevice.
	// Most parameters are zeroed out.
	// SwapEffect is "discard", which is the most efficient method of
	//	presenting the back buffer to the display.
	// Request a back buffer format that matches the current desktop display 
    // format.

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;

	d3dpp.hDeviceWindow  = hWnd;

    // Create the Direct3D device.
	// Default adapter = only one display card.
	//
	// Specify mixed HW/SW processing for HW where appropriate, SW where 
	//	emulation is needed.

    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_MIXED_VERTEXPROCESSING,
                                      &d3dpp, &m_pd3dDevice ) ) )
    {
		assert( false );
        return E_FAIL;
    }

    // Device state would normally be set here

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InitD3D_DestAlpha()
// Desc: Initializes DX8 Direct3D with 8 bits of alpha in the back buffer
//   for rendering effects using destination alpha blending
//-----------------------------------------------------------------------------

HRESULT D3D8Manager::InitD3D_DestAlpha( HWND hWnd, BOOL windowed )
{
	assert( hWnd != NULL );

    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( m_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
	{
		assert( false );
        return E_FAIL;
	}

    // Get the current desktop display mode
    D3DDISPLAYMODE d3ddm;
    if( FAILED( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		assert( false );
        return E_FAIL;
	}

    // Set up the structure used to create the D3DDevice.
	// Most parameters are zeroed out.
	// SwapEffect is "discard", which is the most efficient method of
	//	presenting the back buffer to the display.

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    d3dpp.Windowed			= windowed;
    d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount	= 1;

	if( !windowed )
	{
		RECT r;
		GetWindowRect( hWnd, &r );

		// hWnd Window size must be valid for the sizes available
		//  through D3D!
		// Also hWnd must be top-level for fullscreen

		d3dpp.BackBufferWidth	= r.right;
		d3dpp.BackBufferHeight	= r.bottom;
	}

	d3dpp.hDeviceWindow  = hWnd;
	d3dpp.FullScreen_PresentationInterval =	D3DPRESENT_INTERVAL_DEFAULT;


	//*******************************************
	// get 8 bits of alpha into the back buffer format:

    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;		// for using Destalpha 

	//@@@@@ Uncomment this to see diff in perf under win98:
	// d3dpp.BackBufferFormat = d3ddm.Format;		// format same as desktop


	//*******************************************

    // Create the Direct3D device.
	// Default adapter = only one display card.
	//
	// Specify mixed HW/SW processing for HW where appropriate, SW where 
	//	emulation is needed.
	//
	// NOTE:  I AM NOT creating a DEPTH BUFFER for this.  
	//		There's no reason for that.  Maybe I'll put one in eventually.
	//
	////////////////////

    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
									  hWnd,		// focus window
                                      D3DCREATE_MIXED_VERTEXPROCESSING,
                                      &d3dpp,					// input
									  &m_pd3dDevice ) ) )		// func return
    {

		m_pD3D->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			                     D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, TRUE );


		assert( false );
        return E_FAIL;
    }


    return S_OK;
}
