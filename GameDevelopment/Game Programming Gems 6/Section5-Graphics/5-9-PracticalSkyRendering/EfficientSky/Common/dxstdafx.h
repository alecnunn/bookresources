//--------------------------------------------------------------------------------------
// File: DxStdAfx.h
//
// Desc: Standard includes and precompiled headers for DXUT
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXSDK_STDAFX_H
#define DXSDK_STDAFX_H

#ifndef STRICT
#define STRICT
#endif

// If app hasn't choosen, set to work with Windows 98, Windows Me, Windows 2000, Windows XP and beyond
#ifndef WINVER
#define WINVER         0x0410
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410 
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0500 
#endif

// #define DXUT_AUTOLIB to automatically include the libs needed for DXUT 
#ifdef DXUT_AUTOLIB
#pragma comment( lib, "dxerr9.lib" )
#pragma comment( lib, "dxguid.lib" )
#if defined(DEBUG) | defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )
#endif

#pragma warning( disable : 4100 ) // disable unreference formal parameter warnings for /W4 builds

#include <windows.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <new.h>      // for placement new
#include <math.h>      
#include <limits.h>      
#include <stdio.h>

// CRT's memory leak detection
#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#endif

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

// DirectSound includes
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

// strsafe.h deprecates old unsecure string functions.  If you 
// really do not want to it to (not recommended), then uncomment the next line 
//#define STRSAFE_NO_DEPRECATE

#ifndef STRSAFE_NO_DEPRECATE
#pragma deprecated("strncpy")
#pragma deprecated("wcsncpy")
#pragma deprecated("_tcsncpy")
#pragma deprecated("wcsncat")
#pragma deprecated("strncat")
#pragma deprecated("_tcsncat")
#endif

#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 ) 

#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTenum.h"
#include "DXUTmesh.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "DXUTSound.h"

#if defined(DEBUG) | defined(_DEBUG)
#ifndef V
#define V(x)           { hr = x; if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#else
#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

////////////////////////////////////////////////////////////////////
// AReis:
////////////////////////////////////////////////////////////////////

// Proper C++ made easy.
#define DEFINE_GETSET_ACCESSOR( type, var, name )										\
	type Get##name() { return var; }													\
	void Set##name( type otherVar ) { var = otherVar; }

#define DEFINE_GETSET_ACCESSOR_REFERENCE( type, var, name )								\
	type &Get##name() { return var; }													\
	void Set##name( type &otherVar ) { var = otherVar; }

#define DEFINE_GETSET_ACCESSOR_POINTER( type, var, name )								\
	type *Get##name() { return var; }													\
	void Set##name( type *otherVar ) { var = otherVar; }

#define SETBIT( x ) ( 1 << x )

// Skip warning about truncating void pointer (I meant to do that).
#pragma warning( disable : 4311 )

// Skip warning about depreciated functions (thanks anyways dx).
#pragma warning( disable : 4995 )

// Gah, shut stl up.
#pragma warning( disable : 4530 )
#pragma warning( disable : 80 )
#pragma warning( disable : 83 )
#pragma warning( disable : 804 )

extern HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, const WCHAR * strFileName, 
				 const D3DVERTEXELEMENT9* aMeshDecl,
				 LPDIRECT3DVERTEXBUFFER9* ppVB,LPDIRECT3DINDEXBUFFER9* ppIB, 
				 int *pNumPolys, int *pNumVerts, int *pNumIndices );
extern IDirect3DDevice9 *g_pd3dDevice;
extern IDirect3DVertexDeclaration9 *g_pDecl;

// Build all the pre-computed lookup textures and save them out.
void BuildLookupsAndSave();

//#define USE_MODEL_CAMERA
//#define USE_TEST_CUBEMAP
//#define USE_SH_CONVOLVE

extern D3DXMATRIXA16			g_mWorldFix;
#ifdef USE_MODEL_CAMERA
extern CModelViewerCamera		g_Camera;
#else
extern CFirstPersonCamera		g_Camera;
#endif
extern LPDIRECT3DCUBETEXTURE9	g_pSkyCubeTexture;
extern LPDIRECT3DCUBETEXTURE9	g_pDiffuseIrradianceSkyCubeTexture;
extern LPDIRECT3DCUBETEXTURE9	g_pTestCubeMap;
extern LPDIRECT3DCUBETEXTURE9	g_pStarCubeTexture;

extern LPDIRECT3DSURFACE9		g_pRenderCubeDepthStencil;
extern LPD3DXEFFECT				g_pSkyEffect;
extern LPD3DXEFFECT				g_pParaboloidEffect;
extern CDXUTDirectionWidget		g_LightControl;
extern D3DXHANDLE				g_hCubeMapPassthrough;
extern D3DXHANDLE				g_hRenderDepth, g_hRenderFog;
extern float					g_fTime;
extern int						g_iCurHour;

// The Effect Handles.
extern D3DXHANDLE				g_hLocalEyePos,
								g_hSunLightDir, g_hSunLightColor,
								g_hMoonLightDir, g_hMoonLightColor,
								g_hWorldViewProjection,
								g_hTime, g_hPixelSky, g_hVertexSky, g_hRenderTerrain;

extern IDirect3DTexture9		*g_pTerrainDiffuseMap;
extern IDirect3DTexture9		*g_pTerrainNormalMap;

#define PARABOLOID_SAMPLES 64
#define NUM_ORDER 3
#define NUM_ORDER_P2 4
#define NUM_RESULT_SAMPLES 32     // bilinear filtering of 64x64 texture results in 4xAA 4 free
#define NUM_RADIANCE_SAMPLES 32

extern LPDIRECT3DTEXTURE9		g_pParaboloidMap[ 2 ];
extern LPDIRECT3DTEXTURE9		g_pIrradianceSHCoefficients;

extern LPDIRECT3DTEXTURE9		g_pParaboloidSHWeights[ 2 ];
extern LPDIRECT3DTEXTURE9		g_pLambertSHEval[ 6 ];
extern LPDIRECT3DTEXTURE9		g_pPhongSHEval[ 6 ];

extern LPDIRECT3DTEXTURE9		g_pFunctionFTexture;

extern LPDIRECT3DTEXTURE9		g_pSkyNormalMapLo;
extern LPDIRECT3DTEXTURE9		g_pSkyNormalMapMd;
extern LPDIRECT3DTEXTURE9		g_pSkyNormalMapHi;
extern LPDIRECT3DTEXTURE9		g_pSunTexture;
extern LPDIRECT3DTEXTURE9		g_pMoonTexture;

// Send a message to the debugger.
void PrintMessage( const char *strFormat, ... );

// Format and copy a string with variable argument into a temp buffer.
__inline const char *VarArg( char *strFormat, ... )
{
	static int g_iVAIndex = 0;
	static char g_strVAString[ 2 ][ 32000 ];
	char *strBuffer = g_strVAString[ g_iVAIndex++ & 1 ];
	va_list arglist;

	va_start( arglist, strFormat );
	_vsnprintf( strBuffer, 32000, strFormat, arglist );
	va_end( arglist );

	return strBuffer;
}

namespace AURELIO_REIS {};
using namespace AURELIO_REIS;

#include <vector>
using namespace std;

#endif // !defined(DXSDK_STDAFX_H)
