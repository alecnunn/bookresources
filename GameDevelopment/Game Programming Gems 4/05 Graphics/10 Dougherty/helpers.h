//-----------------------------------------------------------------------------
// File: DynamicGammaHelpers.h
// Desc: Contains app helper functions and macros
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


#ifndef DYNAMIC_GAMMA_HELPERS_H
#define DYNAMIC_GAMMA_HELPERS_H


//-----------------------------------------------------------------------------
// Helper Macros
//-----------------------------------------------------------------------------
#define SAFE_RELEASE( p )       { if(p) { (p)->Release(); (p) = NULL; } }
#define VERIFY( D3DCall )       { HRESULT hr = D3DCall; if( FAILED( hr ) ) FatalD3DError( hr ); }


//-----------------------------------------------------------------------------
// DirectX 9 globals initialized by InitD3D
//-----------------------------------------------------------------------------
extern IDirect3DDevice9* g_pD3DDevice;
extern HWND g_hWnd;
extern IDirect3D9* g_pD3D;
extern D3DPRESENT_PARAMETERS g_PresentParameters;


//-----------------------------------------------------------------------------
// Initializes and cleans up DirectX 9
//-----------------------------------------------------------------------------
void InitD3D();
void FinalizeD3D();


//-----------------------------------------------------------------------------
// prints and error message and kills the app
//-----------------------------------------------------------------------------
void __cdecl FatalError( const CHAR* buf, ... );
void FatalD3DError( HRESULT hr );


//-----------------------------------------------------------------------------
// renders a texture to the screen
//-----------------------------------------------------------------------------
void RenderTexture( IDirect3DTexture9* pTexture,
                    float fX1, float fY1,
                    float fX2, float fY2,
                    BOOL FixedFunctionPixelShader = TRUE,
                    BOOL bEnableAlpha = FALSE );


#endif  //DYNAMIC_GAMMA_HELPERS_H
