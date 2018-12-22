//-----------------------------------------------------------------------------
// File: DynamicGammaHelpers.cpp
// Desc: Contains app helper function implementations
//
// Copyright © 2003 Michael Dougherty (mdougher@hotmail.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.
//
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "helpers.h"


//-----------------------------------------------------------------------------
// DirectX 9 globals initialized by InitD3D
//-----------------------------------------------------------------------------
IDirect3DDevice9* g_pD3DDevice   = NULL;
HWND g_hWnd                      = NULL;
IDirect3D9* g_pD3D               = NULL;
D3DPRESENT_PARAMETERS g_PresentParameters  =
{
    640,                        // BackBufferWidth
    480,                        // BackBufferHeight
    D3DFMT_A8R8G8B8,            // BackBufferFormat
    0,                          // BackBufferCount
    D3DMULTISAMPLE_NONE,        // MultiSampleType
    0,                          // MultiSampleQuality
    D3DSWAPEFFECT_DISCARD,      // SwapEffect
    NULL,                       // hDeviceWindow
    TRUE,                       // Windowed
    TRUE,                       // EnableAutoDepthStencil
    D3DFMT_D24S8,               // AutoDepthStencilFormat
    0,                          // Flags
    D3DPRESENT_RATE_DEFAULT,    // FullScreen_RefreshRateInHz
    D3DPRESENT_INTERVAL_ONE,    // PresentationInterval
};


//-----------------------------------------------------------------------------
// Name: InitD3D
// Desc: Initializes D3D and sets the app D3D globals.
//-----------------------------------------------------------------------------
void InitD3D()
{
    assert( g_pD3D == NULL );
    
    // Register the windows class
    WNDCLASS wndClass = { 0, DefWindowProc, 0, 0, NULL, NULL, NULL,
                          (HBRUSH)GetStockObject(WHITE_BRUSH), NULL, "Dynamic Gamma Window" };
    RegisterClass( &wndClass );

    RECT rc;
    DWORD dwBackBufferWidth  = g_PresentParameters.BackBufferWidth;
    DWORD dwBackBufferHeight = g_PresentParameters.BackBufferHeight;
    SetRect( &rc, 0, 0, dwBackBufferHeight, dwBackBufferWidth );

	DWORD dwWindowStyle = 0;

    AdjustWindowRect( &rc, dwWindowStyle, true );

    // Create the render window
    g_hWnd = CreateWindow( "Dynamic Gamma Window", 
                           "Dynamic Gamma Demo", 
                           dwWindowStyle,
                           CW_USEDEFAULT, 
                           CW_USEDEFAULT,
                           dwBackBufferWidth, 
                           dwBackBufferHeight, 
                           0, 
                           NULL, 
                           NULL, 
                           0 );
    if( g_hWnd == NULL )
        FatalError( "Could not create the app window." );
        
    g_PresentParameters.hDeviceWindow = g_hWnd;

    // Make sure the window lacks a menu 
    SetMenu( g_hWnd, NULL );

    // show the window
    ShowWindow( g_hWnd, SW_SHOW );

    // Create the Direct3D object
    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if( g_pD3D == NULL )
        FatalError( "Could not create the Direct3D9 object." );
    
    VERIFY(
    g_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                          D3DDEVTYPE_HAL, //D3DDEVTYPE_REF, 
                          g_hWnd,
                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &g_PresentParameters,
                          &g_pD3DDevice ) );
    
}


//-----------------------------------------------------------------------------
// Name: FinalizeD3D
//-----------------------------------------------------------------------------
void FinalizeD3D()
{
    SAFE_RELEASE( g_pD3DDevice );
    SAFE_RELEASE( g_pD3D );
    
    if( g_hWnd )
    {
        DestroyWindow( g_hWnd );
        g_hWnd = NULL;
    }
}


//-----------------------------------------------------------------------------
// Name: FatalError
// Desc: Brings down D3D, shows an error message in a message box, and exits.
//-----------------------------------------------------------------------------
void __cdecl FatalError( const CHAR* buf, ... )
{
    FinalizeD3D();
    CHAR strBuffer[1024];

    va_list arglist;
    va_start( arglist, buf );
    _vsnprintf( strBuffer, sizeof(strBuffer), buf, arglist );
    va_end( arglist );

    strBuffer[sizeof(strBuffer)-1] = '\0';

    ShowCursor( TRUE );
    MessageBox( 0, strBuffer, "Fatal Error", MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_ICONERROR );
    exit( 1 );
}


//-----------------------------------------------------------------------------
// Name: FatalD3DError
// Desc: Grabs an hr description and calls FatalError
//-----------------------------------------------------------------------------
void FatalD3DError( HRESULT hr )
{
    const CHAR* szErrorString = DXGetErrorString9( hr );
    FatalError("Direct3D Error.\nhr = %s", szErrorString);
}


//-----------------------------------------------------------------------------
// Name: RenderTexture
// Desc: Renders a texture to the supplied screen space coordinates.
//       Uses fixed function pixel shading if requested, otherwise the
//       current pixel shader set to D3D is used
//-----------------------------------------------------------------------------
void RenderTexture( IDirect3DTexture9* pTexture,
                    float fX1, float fY1,
                    float fX2, float fY2,
                    BOOL FixedFunctionPixelShader,
                    BOOL bEnableAlpha )
{
    //Set render states
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ) );

    // enable or disable alpha
    if( bEnableAlpha )
    {
        VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );
        VERIFY( g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
        VERIFY( g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );
    }
    else
    {
        VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) );
    }

    // set up texture coords
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE ) );  
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 ); );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

    // point filtering
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0 ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) );

    // fixed function transform
    VERIFY( g_pD3DDevice->SetVertexShader( NULL ) );

    // set texture
    VERIFY( g_pD3DDevice->SetTexture( 0, pTexture) );
    
    // set up fixed function shader if a pixel shader is not already set
    if( FixedFunctionPixelShader == TRUE )
    {
        VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 ) );
        VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) );
        if( bEnableAlpha )
        {
            VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 ) );
            VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ) );
        }
        else
        {
            VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE ) );
        }
        VERIFY( g_pD3DDevice->SetPixelShader( NULL ) );
    }

    // For screen-space texture-mapped quadrilateral
    VERIFY( g_pD3DDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 ) );
    VERIFY( g_pD3DDevice->SetVertexShader( NULL ) );

    // Prepare quadrilateral vertices
    FLOAT x0 = fX1;
    FLOAT y0 = fY1;
    FLOAT x1 = fX2;
    FLOAT y1 = fY2;
    
    // Draw a quad 
    FLOAT tu0 = 0.0f;
    FLOAT tv0 = 0.0f;
    FLOAT tu1 = 1.0f;
    FLOAT tv1 = 1.0f;

    struct QUADVERTEX
    {
        FLOAT x, y, z, w;
        FLOAT tu, tv;
    };
    QUADVERTEX aQuad[6] =
    {   // X   Y   Z     W    tu   tv
        { x0, y0, 1.0f, 1.0f, tu0, tv0 },
        { x1, y0, 1.0f, 1.0f, tu1, tv0 },
        { x1, y1, 1.0f, 1.0f, tu1, tv1 },

        { x0, y0, 1.0f, 1.0f, tu0, tv0 },
        { x1, y1, 1.0f, 1.0f, tu1, tv1 },
        { x0, y1, 1.0f, 1.0f, tu0, tv1 },
    };

    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, aQuad, sizeof(QUADVERTEX) ) ); 
}
