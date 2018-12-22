//--------------------------------------------------------------------------------------
// File: EfficientSky.cpp
//
// Created by: Aurelio Reis
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"

#include "include/SceneManager.h"
#include "include/Entity.h"

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------

void    InitApp();
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
HRESULT LoadMesh2( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, const WCHAR * strFileName, 
				 const D3DVERTEXELEMENT9* aMeshDecl,
				 LPDIRECT3DVERTEXBUFFER9* ppVB,LPDIRECT3DINDEXBUFFER9* ppIB, 
				 int *pNumPolys, int *pNumVerts, int *pNumIndices );
void    RenderText( double fTime );
HRESULT RenderLightArrow( D3DXVECTOR3 lightDir, D3DXCOLOR arrowColor );


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

ID3DXFont*				g_pFont	= NULL;			// Font	for	drawing	text
ID3DXSprite*			g_pSprite =	NULL;		// Sprite for batching draw	text calls
bool					g_bShowHelp	= true;		// If true,	it renders the UI control text
#ifdef USE_MODEL_CAMERA
CModelViewerCamera		g_Camera;				// A model viewing camera
#else
CFirstPersonCamera		g_Camera;				// A first person camera
#endif
LPD3DXEFFECT			g_pSkyEffect = NULL;
ID3DXMesh*				g_pMesh	= NULL;
ID3DXMesh*				g_pArrowMesh = NULL;

LPD3DXEFFECT			g_pParaboloidEffect = NULL;

LPDIRECT3DCUBETEXTURE9	g_pTestCubeMap	= NULL;
LPDIRECT3DCUBETEXTURE9	g_pSkyCubeTexture = NULL;
LPDIRECT3DCUBETEXTURE9	g_pDiffuseIrradianceSkyCubeTexture = NULL;
LPDIRECT3DCUBETEXTURE9	g_pStarCubeTexture = NULL;

LPDIRECT3DSURFACE9		g_pRenderCubeDepthStencil = NULL;

LPDIRECT3DTEXTURE9		g_pTerrainDiffuseMap = NULL;
LPDIRECT3DTEXTURE9		g_pTerrainNormalMap = NULL;
LPDIRECT3DTEXTURE9		g_pFunctionFTexture = NULL;

LPDIRECT3DTEXTURE9		g_pIrradianceSHCoefficients = NULL;
LPDIRECT3DTEXTURE9		g_pParaboloidMap[ 2 ] = { NULL, NULL };

LPDIRECT3DTEXTURE9		g_pParaboloidSHWeights[ 2 ] = { NULL, NULL };
LPDIRECT3DTEXTURE9		g_pLambertSHEval[ 6 ] = { NULL, NULL, NULL, NULL, NULL, NULL };
LPDIRECT3DTEXTURE9		g_pPhongSHEval[ 6 ] = { NULL, NULL, NULL, NULL, NULL, NULL };

LPDIRECT3DTEXTURE9		g_pSkyNormalMapLo = NULL, g_pSkyNormalMapMd = NULL, g_pSkyNormalMapHi = NULL;
LPDIRECT3DTEXTURE9		g_pSunTexture = NULL;
LPDIRECT3DTEXTURE9		g_pMoonTexture = NULL;

int						g_iCurHour = 12;

CDXUTDialog				g_HUD;					// manages the 3D UI
CDXUTDialog				g_SampleUI;				// dialog for sample specific controls
bool					g_bEnablePreshader;		// if TRUE,	then D3DXSHADER_NO_PRESHADER is	used when compiling	the	shader
D3DXMATRIXA16			g_mWorldFix;
CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg         g_SettingsDlg;          // Device settings dialog

#define		MAX_STEPS	128

CDXUTDirectionWidget	g_LightControl;
float					g_fSpecularExponent	= 512.0f;
int						g_iCurrentMaterial = 0;
IDirect3DDevice9		*g_pd3dDevice =	NULL;
float					g_fTime;
const int				g_iRenderCubeSize = 512;

// The Effect Handles.
D3DXHANDLE				g_hLocalEyePos = NULL,
						g_hSunLightDir = NULL, g_hSunLightColor = NULL,
						g_hMoonLightDir = NULL, g_hMoonLightColor = NULL,
						g_hWorldViewProjection = NULL,
						g_hTime = NULL,
						g_hPixelSky = NULL, g_hVertexSky = NULL, g_hRenderTerrain = NULL,
						g_hCubeMapPassthrough = NULL,
						g_hRenderDepth = NULL,
						g_hRenderFog = NULL;

CSceneView				*g_pMainView = NULL,				// The 3D main view.
						*g_pCubeMapSubView = NULL,			// The view that draws the sky to the cubemap.
						*g_pParaboloidSubView = NULL;		// The view that creates the irradiance map from sky cubemap.

CD3DModel				g_BoxModel;
CD3DModel				g_DomeModels[ 3 ];
CD3DModel				g_TerrainModel;

CBoxEntity				*g_pBoxEntity = NULL;
CStaticEntity			*g_pTerrainEntity = NULL;

// Vertex decl for meshes.
IDirect3DVertexDeclaration9 *g_pDecl = NULL;

// Create a new vertex declaration to hold all the required data.
const D3DVERTEXELEMENT9 g_BoxVertexDecl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
	{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
	{ 0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};


const D3DVERTEXELEMENT9 g_DunesVertexDecl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
	{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
	{ 0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};


const D3DVERTEXELEMENT9 g_DomeVertexDecl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
	{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
	{ 0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 56, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
	D3DDECL_END()
};


struct DomeVertex
{
	D3DXVECTOR3	vPosition;
	D3DXVECTOR3	vNormal;
	D3DXVECTOR3	vTangent;
	D3DXVECTOR3	vBinormal;
	D3DXVECTOR2	vTC0;
	float		fOpticalDepth;
};


CSceneManager g_SceneManager;
CSceneManager *CSceneManager::m_pInstance = &g_SceneManager;


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------

#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_ENABLE_PRESHADER    5

#define IDC_CURRENT_DOME				6
#define IDC_CURRENT_DOME_STATIC			7
#define IDC_CURRENT_DOME_STATIC2		8	
#define IDC_USE_CUBIC_SKYDOME			9
#define IDC_SKY_TECHNIQUE				10
#define IDC_SKY_HOUR					11
#define IDC_SKY_HOUR_STATIC				12
#define IDC_WIREFRAME					13


//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
								 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	// Skip backbuffer formats that don't support alpha blending.
	IDirect3D9* pD3D = DXUTGetD3DObject(); 
	if ( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
		D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
	{
		return false;
	}

	// Must support cube textures.
	if ( !( pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP ) )
	{
		return false;
	}

	// Must support pixel shader 2.0.
	if ( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
	{
		return false;
	}

	// Must support D3DFMT_A8R8G8B8 render target.
	if ( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_RENDERTARGET, 
		D3DRTYPE_CUBETEXTURE, D3DFMT_A8R8G8B8 ) ) )
	{
		return false;
	}

	return true;
}

void SetUseCubicSkyDomeMap( bool bUseCSDMap )
{
	if ( bUseCSDMap )
	{
		g_pCubeMapSubView->SetEnabled( true );

		// Only draw for view 0.
		g_pBoxEntity->m_ulSceneViewMask = VIEWID_0;
		g_pBoxEntity->SetVisible( true );

		// Only draw for view 1.
		g_pSkyEntity->m_ulSceneViewMask = VIEWID_1;
	}
	else
	{
		g_pCubeMapSubView->SetEnabled( false );

		// Don't draw the Skybox.
		g_pBoxEntity->m_ulSceneViewMask = VIEWID_0;
		g_pBoxEntity->SetVisible( false );

		// Only draw for view 0.
		g_pSkyEntity->m_ulSceneViewMask = VIEWID_0;
	}
}

//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	// If device doesn't support HW T&L or doesn't support 2.0 vertex shaders in HW 
	// then switch to SWVP.
	if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
		pCaps->VertexShaderVersion < D3DVS_VERSION(2,0) )
	{
		pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	else
	{
		pDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}

	// This application is designed to work on a pure device by not using 
	// IDirect3D9::Get*() methods, so create a pure device if supported and using HWVP.
	if ((pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 && 
		(pDeviceSettings->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 )
		pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;

	// Debugging vertex shaders requires either REF or software vertex processing 
	// and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
	if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
	{
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
		pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
#endif
#ifdef DEBUG_PS
	pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning();
    }

	return true;
}

DWORD g_dwShaderFlags = 0;

//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	V_RETURN( g_DialogResourceManager.OnCreateDevice( pd3dDevice ) );
	V_RETURN( g_SettingsDlg.OnCreateDevice( pd3dDevice ) );

	// Initialize the font
	V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		L"Arial", &g_pFont ) );

	g_pd3dDevice = pd3dDevice;

#ifdef DEBUG_VS
	g_dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	g_dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	// Preshaders are parts of the shader that the effect system pulls out of the 
	// shader and runs on the host CPU. They should be used if you are GPU limited. 
	// The D3DXSHADER_NO_PRESHADER flag disables preshaders.
	if( !g_bEnablePreshader )
		g_dwShaderFlags |= D3DXSHADER_NO_PRESHADER;

	// Read the D3DX effect file
	WCHAR str[MAX_PATH];
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"EfficientSky.fx" ) );

	// If this fails, there should be debug output as to 
	// why the .fx file failed to compile
	LPD3DXBUFFER pCompilationErrors;
	hr = D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, g_dwShaderFlags, NULL, &g_pSkyEffect, &pCompilationErrors );

	if ( FAILED( hr ) )
	{
		WCHAR wsz[ 256 ];
		MultiByteToWideChar( CP_ACP, 0, (LPSTR)pCompilationErrors->GetBufferPointer(), -1, wsz, 256 );
		wsz[ 255 ] = 0;
		DXUTTrace( __FILE__, (DWORD)__LINE__, E_FAIL, wsz, true );

		return hr;
	}

#ifdef USE_SH_CONVOLVE
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"ParaboloidConvolve.fx" ) );

	hr = D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, g_dwShaderFlags, NULL, &g_pParaboloidEffect, &pCompilationErrors );

	if ( FAILED( hr ) )
	{
		WCHAR wsz[ 256 ];
		MultiByteToWideChar( CP_ACP, 0, (LPSTR)pCompilationErrors->GetBufferPointer(), -1, wsz, 256 );
		wsz[ 255 ] = 0;
		DXUTTrace( __FILE__, (DWORD)__LINE__, E_FAIL, wsz, true );

		return hr;
	}
#endif

	g_hLocalEyePos = g_pSkyEffect->GetParameterByName( NULL, "g_vLocalEyePos" );
	g_hSunLightDir = g_pSkyEffect->GetParameterByName( NULL, "g_SunLightDir" );
	g_hSunLightColor = g_pSkyEffect->GetParameterByName( NULL, "g_SunLightColor" );
	g_hMoonLightDir = g_pSkyEffect->GetParameterByName( NULL, "g_MoonLightDir" );
	g_hMoonLightColor = g_pSkyEffect->GetParameterByName( NULL, "g_MoonLightColor" );
	g_hWorldViewProjection = g_pSkyEffect->GetParameterByName( NULL, "g_mWorldViewProjection" );
	g_hTime = g_pSkyEffect->GetParameterByName( NULL, "g_fTime" );
	g_hRenderDepth = g_pSkyEffect->GetTechniqueByName( "RenderDepth" );
	g_hRenderFog = g_pSkyEffect->GetTechniqueByName( "RenderFog" );
	g_hPixelSky = g_pSkyEffect->GetTechniqueByName( "RenderPixelSky" );
	g_hVertexSky = g_pSkyEffect->GetTechniqueByName( "RenderVertexSky" );
	g_hRenderTerrain = g_pSkyEffect->GetTechniqueByName( "RenderTerrain" );
	g_hCubeMapPassthrough = g_pSkyEffect->GetTechniqueByName( "RenderCubeMapPassthrough" );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
							   const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	// Build the pre-computed textures.
	BuildLookupsAndSave();

	HRESULT hr;
	WCHAR str[MAX_PATH];

	V_RETURN( g_DialogResourceManager.OnResetDevice() );
	V_RETURN( g_SettingsDlg.OnResetDevice() );

	if( g_pFont )
		V_RETURN( g_pFont->OnResetDevice() );
	if( g_pSkyEffect )
		V_RETURN( g_pSkyEffect->OnResetDevice() );

	// Create a sprite to help batch calls when drawing many lines of text
	V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite ) );

	g_LightControl.OnResetDevice( pBackBufferSurfaceDesc  );

	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 1.0f, 5000.0f );
#ifdef USE_MODEL_CAMERA
	g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	g_Camera.SetButtonMasks( MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON );
#endif

	g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
	g_HUD.SetSize( 170, 170 );
	g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-170, pBackBufferSurfaceDesc->Height-300 );
	g_SampleUI.SetSize( 170, 300 );

	////////////////////////////////////////////////////////////////////////////
	// Load the Models.
	////////////////////////////////////////////////////////////////////////////

	if ( !g_BoxModel.LoadModel( L"box.x", g_BoxVertexDecl ) )
	{
		return E_FAIL;
	}

	if ( !g_DomeModels[ 0 ].LoadModel( L"hemi_low.x", g_DomeVertexDecl ) )
	{
		return E_FAIL;
	}

	if ( !g_DomeModels[ 1 ].LoadModel( L"hemi_med.x", g_DomeVertexDecl ) )
	{
		return E_FAIL;
	}

	if ( !g_DomeModels[ 2 ].LoadModel( L"hemi_high.x", g_DomeVertexDecl ) )
	{
		return E_FAIL;
	}

	if ( !g_TerrainModel.LoadModel( L"dunes_low.x", g_DunesVertexDecl ) )
	{
		return E_FAIL;
	}

	// Precalculate any constants for the Dome Models.
	const float fInv = 1.0f / 5.0f;
	for ( int i = 0; i < 3; i++ )
	{
		DomeVertex *pVertices;
		g_DomeModels[ i ].LockVertices( (void **) &pVertices );

		int iNumVerts = g_DomeModels[ i ].GetNumVerts();
		for ( int j = 0; j < iNumVerts; j++, ++pVertices )
		{
			pVertices->fOpticalDepth = 1.65f - powf( fabs( pVertices->vNormal.y ), fInv );
		}

		g_DomeModels[ i ].UnlockVertices();
	}

	V_RETURN( g_pd3dDevice->CreateCubeTexture(	g_iRenderCubeSize, 1, D3DUSAGE_RENDERTARGET,
												D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pSkyCubeTexture, NULL ) );

	DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();
	V_RETURN( g_pd3dDevice->CreateDepthStencilSurface(	g_iRenderCubeSize, g_iRenderCubeSize,
														d3dSettings.pp.AutoDepthStencilFormat,
														d3dSettings.pp.MultiSampleType,
														d3dSettings.pp.MultiSampleQuality, TRUE,
														&g_pRenderCubeDepthStencil, NULL ) );

	g_pCubeMapSubView->m_RenderTarget.m_pRenderTarget = g_pSkyCubeTexture;
	g_pCubeMapSubView->m_RenderTarget.m_pRenderTargetDepthStencil = g_pRenderCubeDepthStencil;

	// For testing.
#ifdef USE_TEST_CUBEMAP
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"testcubemap.dds" ) );
	V_RETURN( D3DXCreateCubeTextureFromFile( g_pd3dDevice, str, &g_pTestCubeMap ) );
#endif

	// Load the Star Cube Map.
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"starcube.dds" ) );
	V_RETURN( D3DXCreateCubeTextureFromFile( g_pd3dDevice, str, &g_pStarCubeTexture ) );

	// Load the sky normal maps.
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"skydome_lo_n.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pSkyNormalMapLo ) )

	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"skydome_md_n.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pSkyNormalMapMd ) )

	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"skydome_hi_n.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pSkyNormalMapHi ) )

	// The diffuse and normal map for the terrain model.
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"dunes_d.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pTerrainDiffuseMap ) )

	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"dunes_n.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pTerrainNormalMap ) )

	V_RETURN( LoadMesh2( pd3dDevice, L"UI\\arrow.x", &g_pArrowMesh ) );

	// Load the F Texture.
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"media/lookups/F.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pFunctionFTexture ) )

	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"media/sun.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pSunTexture ) )

	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"media/moon.dds" ) );
	V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
											D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
											D3DX_DEFAULT, D3DX_DEFAULT, 0, 
											NULL, NULL, &g_pMoonTexture ) )

	// Create the target paraboloid and convolved cube textures.
	for ( int i = 0; i < 2; i++ )
	{
		V_RETURN( g_pd3dDevice->CreateTexture( PARABOLOID_SAMPLES, PARABOLOID_SAMPLES, 1, D3DUSAGE_RENDERTARGET,
											   D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &g_pParaboloidMap[ i ], NULL) );
	}
    
	V_RETURN( pd3dDevice->CreateCubeTexture( NUM_RESULT_SAMPLES, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
											 D3DPOOL_DEFAULT, &g_pDiffuseIrradianceSkyCubeTexture, NULL ) );

	//V_RETURN( pd3dDevice->CreateCubeTexture( NUM_RESULT_SAMPLES, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
	//										 D3DPOOL_DEFAULT, &g_pSpecularIrradianceSkyCube, NULL ) );

	V_RETURN( pd3dDevice->CreateTexture( NUM_ORDER_P2, NUM_ORDER_P2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
										 D3DPOOL_DEFAULT, &g_pIrradianceSHCoefficients, NULL ) );

	// Load all the Irradiance Images.
	for ( int i = 0; i < 2; i++ )
	{
		StringCchPrintf( str, MAX_PATH, L"media/lookups/ParaboloidSHWeights%d.dds", i );
		V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, str ) );
		V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
												D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
												D3DX_DEFAULT, D3DX_DEFAULT, 0, 
												NULL, NULL, &g_pParaboloidSHWeights[ i ] ) )
	}

	for ( int i = 0; i < 6; i++ )
	{
		StringCchPrintf( str, MAX_PATH, L"media/lookups/LambertSHEval%d.dds", i );
		V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, str ) );
		V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
												D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
												D3DX_DEFAULT, D3DX_DEFAULT, 0, 
												NULL, NULL, &g_pLambertSHEval[ i ] ) )
	}

	for ( int i = 0; i < 6; i++ )
	{
		StringCchPrintf( str, MAX_PATH, L"media/lookups/SpecularSHEval%d.dds", i );
		V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, str ) );
		V_RETURN( D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
												D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
												D3DX_DEFAULT, D3DX_DEFAULT, 0, 
												NULL, NULL, &g_pPhongSHEval[ i ] ) )
	}

	const D3DXVECTOR3 &vCenter = g_BoxModel.GetCenter();
	float fRadius = g_BoxModel.GetRadius();

	D3DXMatrixTranslation( &g_mWorldFix, -vCenter.x, -vCenter.y, -vCenter.z );
	D3DXMATRIXA16 m;
	D3DXMatrixRotationY( &m, D3DX_PI );
	g_mWorldFix *= m;
	D3DXMatrixRotationX( &m, D3DX_PI / 2.0f );
	g_mWorldFix *= m;

	V_RETURN( CDXUTDirectionWidget::StaticOnCreateDevice( pd3dDevice ) );
	g_LightControl.SetRadius( fRadius );

	// Setup the camera's view parameters
	D3DXVECTOR3 vecEye(0.0f, -10.0f, -150.0f);
	D3DXVECTOR3 vecAt (0.0f, 0.0f, -0.0f);
	g_Camera.SetViewParams( &vecEye, &vecAt );

#ifdef USE_MODEL_CAMERA
	g_Camera.SetRadius( fRadius*3.0f, fRadius*0.5f, fRadius*10.0f );
#else

	g_Camera.SetClipToBoundary( false, NULL, NULL );
	g_Camera.SetEnableYAxisMovement( true );
    g_Camera.SetRotateButtons( true, false, false );
	g_Camera.SetScalers( 0.01f, 250.0f );
	g_Camera.SetViewParams( &vecEye, &vecAt );
#endif

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	// Update the camera's position based on user input 
	g_Camera.FrameMove( fElapsedTime );
}

//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    // If the settings dialog is being shown, then
    // render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

	g_fTime = (float) fTime;

	// Update the scene
	CSceneManager::m_pInstance->UpdateScene();

	// Render the scene
	CSceneManager::m_pInstance->RenderScene();
	
	pd3dDevice->BeginScene();

	// Render the GUI.
	g_HUD.OnRender( fElapsedTime ); 
	g_SampleUI.OnRender( fElapsedTime );

	pd3dDevice->EndScene();

	// Render any other debug Text.
	RenderText( fTime );
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	if( g_SettingsDlg.IsActive() )
	{
		g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
		return 0;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	g_LightControl.HandleMessages( hWnd, uMsg, wParam, lParam );

	// Pass all remaining windows messages to camera so it can respond to user input
	g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

	return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	g_DialogResourceManager.OnLostDevice();
	g_SettingsDlg.OnLostDevice();
	CDXUTDirectionWidget::StaticOnLostDevice();
	if( g_pFont )
		g_pFont->OnLostDevice();
	if( g_pSkyEffect )
		g_pSkyEffect->OnLostDevice();

	SAFE_RELEASE( g_pSprite );
	SAFE_RELEASE( g_pDecl );
	SAFE_RELEASE( g_pMesh );
	
	SAFE_RELEASE( g_pArrowMesh );

	SAFE_RELEASE( g_pSkyCubeTexture );
	SAFE_RELEASE( g_pRenderCubeDepthStencil );
	SAFE_RELEASE( g_pTestCubeMap );
	SAFE_RELEASE( g_pStarCubeTexture );

	SAFE_RELEASE( g_pSkyNormalMapLo );
	SAFE_RELEASE( g_pSkyNormalMapMd );
	SAFE_RELEASE( g_pSkyNormalMapHi );

	SAFE_RELEASE( g_pTerrainDiffuseMap );
	SAFE_RELEASE( g_pTerrainNormalMap );

	SAFE_RELEASE( g_pSunTexture );
	SAFE_RELEASE( g_pMoonTexture );

	SAFE_RELEASE( g_pFunctionFTexture );

	g_BoxModel.Release();
	for ( int i = 0; i < 3; i++ )
	{
		g_DomeModels[ i ].Release();
	}
	g_TerrainModel.Release();

	// Release all the Irradiance Images.
	for ( int i = 0; i < 2; i++ )
	{
		 SAFE_RELEASE( g_pParaboloidMap[ i ] );
	}
    
	 SAFE_RELEASE( g_pDiffuseIrradianceSkyCubeTexture );
	// SAFE_RELEASE( g_pSpecularIrradianceSkyCube );
	SAFE_RELEASE( g_pIrradianceSHCoefficients );


	for ( int i = 0; i < 2; i++ )
	{
		SAFE_RELEASE( g_pParaboloidSHWeights[ i ] );
	}

	for ( int i = 0; i < 6; i++ )
	{
		SAFE_RELEASE( g_pLambertSHEval[ i ] );
	}

	for ( int i = 0; i < 6; i++ )
	{
		SAFE_RELEASE( g_pPhongSHEval[ i ] );
	}
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	g_DialogResourceManager.OnDestroyDevice();
	g_SettingsDlg.OnDestroyDevice();
	CDXUTDirectionWidget::StaticOnDestroyDevice();

	SAFE_RELEASE( g_pSkyEffect );
	SAFE_RELEASE( g_pFont );
}



//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Set the callback functions
	DXUTSetCallbackDeviceCreated( OnCreateDevice );
	DXUTSetCallbackDeviceReset( OnResetDevice );
	DXUTSetCallbackDeviceLost( OnLostDevice );
	DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( KeyboardProc );
	DXUTSetCallbackFrameRender( OnFrameRender );
	DXUTSetCallbackFrameMove( OnFrameMove );

	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen

	InitApp();

	// Initialize DXUT and create the desired Win32 window and Direct3D device for the application
	DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
	DXUTCreateWindow( L"EfficientSky" );
	DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

	// Start the render loop
	DXUTMainLoop();

	// Release the App.
	CSceneManager::m_pInstance->ReleaseScene();

	return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	g_bEnablePreshader = true;

	g_LightControl.SetLightDirection( D3DXVECTOR3( sinf(D3DX_PI*2/MAX_STEPS-D3DX_PI/6), D3DX_PI / 2.0f, -cosf(D3DX_PI*2*0/MAX_STEPS-D3DX_PI/6) ) );

	g_HUD.Init( &g_DialogResourceManager );
	g_SampleUI.Init( &g_DialogResourceManager );
	g_SettingsDlg.Init( &g_DialogResourceManager );

	// Initialize dialogs
	g_HUD.SetCallback( OnGUIEvent );
	int iY = 10; 
	g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
	g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
	g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );

	g_SampleUI.SetCallback( OnGUIEvent );
	iY = -70; 

	WCHAR sz[100];

	iY += 100;
	StringCchPrintf( sz, 100, L"Dome Model Resolution:\nMedium  (640 polys)" ); 
	g_SampleUI.AddStatic( IDC_CURRENT_DOME_STATIC, sz, -60, iY += -20, 300, 40 );
	g_SampleUI.AddSlider( IDC_CURRENT_DOME, 50, iY += 40, 100, 22, 0, 2, 1 );
	g_SampleUI.AddStatic( IDC_CURRENT_DOME_STATIC2, L"      Low       Med       High", -60, iY += 5, 300, 40 );

	iY += 24;
	StringCchPrintf( sz, 100, L"Hour: %d", g_iCurHour ); 
	g_SampleUI.AddStatic( IDC_SKY_HOUR_STATIC, sz, -80, iY += 12, 300, 40 );
	g_SampleUI.AddSlider( IDC_SKY_HOUR, 50, iY += 32, 100, 22, 0, 23, g_iCurHour );

	iY += 24;
	StringCchPrintf( sz, 100, L"Use Cubic Sky Dome Map (C)" ); 
	g_SampleUI.AddCheckBox( IDC_USE_CUBIC_SKYDOME, sz, -10, iY += 24, 180, 22, false, 'C' );

	iY += 0;
	StringCchPrintf( sz, 100, L"Show Wireframe (F)" ); 
	g_SampleUI.AddCheckBox( IDC_WIREFRAME, sz, -10, iY += 24, 180, 22, false, 'F' );

	iY += 24;
	CDXUTComboBox *pComboBox;
	g_SampleUI.AddComboBox( IDC_SKY_TECHNIQUE, -80, iY += 24, 245, 22, L'T', false, &pComboBox );

	pComboBox->SetDropHeight( 12 );
	pComboBox->AddItem( L"Per-Pixel Sky Color Technique (T)", (LPVOID) 0 );
	pComboBox->AddItem( L"Per-Vertex Sky Color Technique (T)", (LPVOID) 1 );
	pComboBox->SetSelectedByIndex( 0 );

	// Add the main view.
	g_pMainView = new CSceneView();
	g_pMainView->m_ulClearColor = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	g_pMainView->m_ulViewID = VIEWID_0;
	g_pMainView->m_ViewType = VIEWTYPE_NORMAL;
	CSceneManager::m_pInstance->AddView( g_pMainView );

	// Now add the sub-views.
	g_pCubeMapSubView = new CSceneView();
	g_pCubeMapSubView->m_ulViewID = VIEWID_1;
	g_pCubeMapSubView->m_ViewType = VIEWTYPE_CUBE_RTT;
	g_pCubeMapSubView->SetNextRenderDelta( 2.0f );
	g_pMainView->AddSubView( g_pCubeMapSubView );

	// Create the paraboloid view as dependant of the cube map view.
	g_pParaboloidSubView = new CSceneView();
	g_pParaboloidSubView->m_ulViewID = VIEWID_2;
	g_pParaboloidSubView->m_ViewType = VIEWTYPE_PARABOLOID_CONVOLUTION;
	g_pParaboloidSubView->SetNextRenderDelta( 2.0f );
	g_pCubeMapSubView->AddSubView( g_pParaboloidSubView );

	// Add the Entities to the manager.
	// NOTE: I chose to have the scene manager explicitely manage entities,
	// although the proper way would probably be to have an entity or world
	// manager (which I won't present here).
	g_pSkyEntity = new CSkyEntity();
	g_pSkyEntity->SetViewHack( true );
	CSceneManager::m_pInstance->AddEntity( g_pSkyEntity );

	g_pBoxEntity = new CBoxEntity();
	g_pBoxEntity->SetViewHack( true );
	CSceneManager::m_pInstance->AddEntity( g_pBoxEntity );

	g_pTerrainEntity = new CStaticEntity();
	CSceneManager::m_pInstance->AddEntity( g_pTerrainEntity );

	D3DXMATRIX matScale;
	D3DXMatrixScaling( &matScale, 10.0f, 7.0f, 10.0f );

	// Move the terrain down a tad.
	D3DXMatrixTranslation( &g_pTerrainEntity->m_matTransform, 0.0f, -6.0f, 0.0f );
	D3DXMatrixMultiply( &g_pTerrainEntity->m_matTransform, &g_pTerrainEntity->m_matTransform, &matScale );

	// Draw only for the normal view.
	g_pTerrainEntity->m_ulSceneViewMask = VIEWID_0;

	// Start with the Cubic Sky Dome Map off.
	SetUseCubicSkyDomeMap( false );

	g_pBoxEntity->SetModel( &g_BoxModel );
	g_pSkyEntity->SetModel( &g_DomeModels[ 1 ] );
	g_pTerrainEntity->SetModel( &g_TerrainModel );
}

//--------------------------------------------------------------------------------------
// This function loads the mesh and ensures the mesh has normals; it also optimizes the 
// mesh for the graphics card's vertex cache, which improves performance by organizing 
// the internal triangle list for less cache misses.
//--------------------------------------------------------------------------------------
HRESULT LoadMesh2( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh )
{
	ID3DXMesh* pMesh = NULL;
	WCHAR str[MAX_PATH];
	HRESULT hr;

	// Load the mesh with D3DX and get back a ID3DXMesh*.  For this
	// sample we'll ignore the X file's embedded materials since we know 
	// exactly the model we're loading.  See the mesh samples such as
	// "OptimizedMesh" for a more generic mesh loading example.
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFileName ) );
	V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh) );

	DWORD *rgdwAdjacency = NULL;

	// Make sure there are normals which are required for lighting
	if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
	{
		ID3DXMesh* pTempMesh;
		V( pMesh->CloneMeshFVF( pMesh->GetOptions(), 
			pMesh->GetFVF() | D3DFVF_NORMAL, 
			pd3dDevice, &pTempMesh ) );
		V( D3DXComputeNormals( pTempMesh, NULL ) );

		SAFE_RELEASE( pMesh );
		pMesh = pTempMesh;
	}

	// Optimize the mesh for this graphics card's vertex cache 
	// so when rendering the mesh's triangle list the vertices will 
	// cache hit more often so it won't have to re-execute the vertex shader 
	// on those vertices so it will improve perf.     
	rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
	if( rgdwAdjacency == NULL )
		return E_OUTOFMEMORY;
	V( pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
	V( pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL) );
	delete []rgdwAdjacency;

	*ppMesh = pMesh;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// This function loads a model from a .X file and optimizes it in place, and marshals it
//	into a VB following the vertex format passed in.
//--------------------------------------------------------------------------------------
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, const WCHAR * strFileName, 
				 const D3DVERTEXELEMENT9* aMeshDecl,
				 LPDIRECT3DVERTEXBUFFER9* ppVB,LPDIRECT3DINDEXBUFFER9* ppIB, 
				 int *pNumPolys, int *pNumVerts, int *pNumIndices )
{
	ID3DXMesh* pMesh = NULL;
	WCHAR str[MAX_PATH];
	HRESULT hr;

	// Load the mesh with D3DX and get back a ID3DXMesh*.  For this
	// sample we'll ignore the X file's embedded materials since we know 
	// exactly the model we're loading.  See the mesh samples such as
	// "OptimizedMesh" for a more generic mesh loading example.
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFileName ) );
	V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh) );

	DWORD *rgdwAdjacency = NULL;

	// Make sure there are normals which are required for lighting
	if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
	{
		ID3DXMesh* pTempMesh;
		V( pMesh->CloneMeshFVF( pMesh->GetOptions(), 
			pMesh->GetFVF() | D3DFVF_NORMAL, 
			pd3dDevice, &pTempMesh ) );
		V( D3DXComputeNormals( pTempMesh, NULL ) );
		SAFE_RELEASE( pMesh );
		pMesh = pTempMesh;
	}

	rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
	if( rgdwAdjacency == NULL )
		return E_OUTOFMEMORY;

	// Optimize the mesh for this graphics card's vertex cache 
	// so when rendering the mesh's triangle list the vertices will 
	// cache hit more often so it won't have to re-execute the vertex shader 
	// on those vertices so it will improve perf.     
	V( pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
	V( pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL) );
	delete []rgdwAdjacency;

	// Clone the mesh into one that uses our decl
	ID3DXMesh* pConformedMesh = NULL;
	V(pMesh->CloneMesh(pMesh->GetOptions(),aMeshDecl,pd3dDevice,&pConformedMesh));
	pMesh->Release();
	pMesh = pConformedMesh;

	// Update tangent and binorm info
	rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
	if( rgdwAdjacency == NULL )
		return E_OUTOFMEMORY;
	V( pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
	V(D3DXComputeTangent( pMesh, 0,0,0,0,rgdwAdjacency));
	delete []rgdwAdjacency;

	// Create our model VB
	int numMeshVerts = pMesh->GetNumVertices();
	int bufferSize = numMeshVerts*D3DXGetDeclVertexSize( aMeshDecl, 0 );
	V(pd3dDevice->CreateVertexBuffer(bufferSize,D3DUSAGE_WRITEONLY, 0,D3DPOOL_DEFAULT, ppVB, NULL));

	// Fill the vertex buffer with data from the mesh object
	LPVOID pMeshVertices;
	V((*ppVB)->Lock( 0, 0, &pMeshVertices, 0));

	// Pull straight from mesh object VB, they have the same decl so it should be good
	LPVOID pMeshData;
	pMesh->LockVertexBuffer(0,&pMeshData);
	memcpy(pMeshVertices,pMeshData,bufferSize);

	// Release our locks
	V((*ppVB)->Unlock());
	V(pMesh->UnlockVertexBuffer());

	// Now for the IB.  This is silly. Gotta basically memcpy out the IB to replicate it.  
	V(pMesh->GetIndexBuffer(ppIB));
	D3DINDEXBUFFER_DESC ibDesc;
	V((*ppIB)->GetDesc(&ibDesc));
	(*ppIB)->Release();

	int numMeshIndices = ibDesc.Format==D3DFMT_INDEX16?(ibDesc.Size/2):(ibDesc.Size/4);
	int numPolysPerModel = numMeshIndices/3;

	int indexBufferSize = ibDesc.Format==D3DFMT_INDEX16?(numMeshIndices*2):(numMeshIndices*4);
	V(pd3dDevice->CreateIndexBuffer(indexBufferSize,D3DUSAGE_WRITEONLY,ibDesc.Format,D3DPOOL_DEFAULT,ppIB,NULL));

	LPVOID pIBDataSrc;
	LPVOID pIBDataDst;
	V(pMesh->LockIndexBuffer(D3DLOCK_READONLY,&pIBDataSrc));
	V((*ppIB)->Lock(0,indexBufferSize,&pIBDataDst,0));
	memcpy(pIBDataDst,pIBDataSrc,indexBufferSize);
	V(pMesh->UnlockIndexBuffer());
	V((*ppIB)->Unlock());

	SAFE_RELEASE(pMesh);

	// Propagate back to caller
	*pNumIndices = numMeshIndices;
	*pNumPolys = numPolysPerModel;
	*pNumVerts = numMeshVerts;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText( double fTime )
{
	// The helper object simply helps keep track of text position, and color
	// and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
	// If NULL is passed in as the sprite object, then it will work fine however the 
	// pFont->DrawText() will not be batched together.  Batching calls will improves perf.
	CDXUTTextHelper txtHelper( g_pFont, g_pSprite, 15 );

	// Output statistics
	txtHelper.Begin();
	txtHelper.SetInsertionPos( 2, 0 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
	txtHelper.DrawTextLine( DXUTGetFrameStats() );
	txtHelper.DrawTextLine( DXUTGetDeviceStats() );

	// Draw help
	if( g_bShowHelp )
	{
		const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
		txtHelper.SetInsertionPos( 2, pd3dsdBackBuffer->Height-15*6 );
		txtHelper.SetForegroundColor( D3DXCOLOR(1.0f, 0.75f, 0.0f, 1.0f ) );
		txtHelper.DrawTextLine( L"Controls:" );

		txtHelper.SetInsertionPos( 20, pd3dsdBackBuffer->Height-15*5 );
		txtHelper.DrawTextLine( L"Rotate model: Left mouse button\n"
			L"Rotate light: Right mouse button\n"
			L"Rotate camera: Middle mouse button\n"
			L"Zoom camera: Mouse wheel scroll\n" );

		txtHelper.SetInsertionPos( 250, pd3dsdBackBuffer->Height-15*5 );
		txtHelper.DrawTextLine( L"Hide help: F1\n" 
			L"Quit: ESC\n" );
	}
	else
	{
		txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
		txtHelper.DrawTextLine( L"Press F1 for help" );
	}
	txtHelper.End();
}

//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke 
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if( bKeyDown )
	{
		switch( nChar )
		{
		case VK_F1: g_bShowHelp = !g_bShowHelp; break;
		}
	}
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{

	switch( nControlID )
	{
		case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
		case IDC_TOGGLEREF:        DXUTToggleREF(); break;
		case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;

		case IDC_ENABLE_PRESHADER: 
		{
			g_bEnablePreshader = g_SampleUI.GetCheckBox( IDC_ENABLE_PRESHADER )->GetChecked(); 

			if( DXUTGetD3DDevice() != NULL )
			{
				OnLostDevice( NULL );
				OnDestroyDevice( NULL );
				OnCreateDevice( DXUTGetD3DDevice(), DXUTGetBackBufferSurfaceDesc(), NULL );
				OnResetDevice( DXUTGetD3DDevice(), DXUTGetBackBufferSurfaceDesc(), NULL );
			}
			break;
		}

		case IDC_CURRENT_DOME:
		{
			WCHAR sz[100];

			switch ( g_SampleUI.GetSlider( IDC_CURRENT_DOME )->GetValue() )
			{
				default:
				case 0:
					g_pSkyEntity->SetModel( &g_DomeModels[ 0 ] );
					StringCchPrintf( sz, 100, L"Dome Model Resolution:\nLow  (40 polys)" ); 
				break;
				case 1:
					g_pSkyEntity->SetModel( &g_DomeModels[ 1 ] );
					StringCchPrintf( sz, 100, L"Dome Model Resolution:\nMedium  (640 polys)" ); 
				break;
				case 2:
					g_pSkyEntity->SetModel( &g_DomeModels[ 2 ] );
					StringCchPrintf( sz, 100, L"Dome Model Resolution:\nHigh  (1440 polys)" ); 
				break;
			}
			
			g_SampleUI.GetStatic( IDC_CURRENT_DOME_STATIC )->SetText( sz );
		}
		break;

		case IDC_SKY_HOUR:
		{
			WCHAR sz[100];
			g_pCubeMapSubView->SetNextRender( 0.0f );
			g_iCurHour = g_SampleUI.GetSlider( IDC_SKY_HOUR )->GetValue();
			StringCchPrintf( sz, 100, L"Hour: %d", g_iCurHour ); 
			g_SampleUI.GetStatic( IDC_SKY_HOUR_STATIC )->SetText( sz );
		}
		break;

		case IDC_USE_CUBIC_SKYDOME:
		{
			// Start with the Cubic Sky Dome Map off.
			g_pCubeMapSubView->SetNextRender( 0.0f );
			SetUseCubicSkyDomeMap( g_SampleUI.GetCheckBox( IDC_USE_CUBIC_SKYDOME )->GetChecked() );
		}
		break;

		case IDC_SKY_TECHNIQUE:
		{
			unsigned long ulData = (unsigned long) g_SampleUI.GetComboBox( IDC_SKY_TECHNIQUE )->GetSelectedItem()->pData;
			switch ( ulData )
			{
			case 0:
				g_pSkyEntity->m_bUseVertexSky = false;
				break;

			case 1:
				g_pSkyEntity->m_bUseVertexSky = true;
				break;
			}
		}
		break;

		case IDC_WIREFRAME:
		{
			if ( g_SampleUI.GetCheckBox( IDC_WIREFRAME )->GetChecked() )
			{
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			}
			else
			{
				g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			}
		}
		break;
	}
}

// Send a message to the debugger.
void PrintMessage( const char *strFormat, ... )
{
	static char g_strMsgString[ 2048 ];
	va_list arglist;

	va_start( arglist, strFormat );
	_vsnprintf( g_strMsgString, 2048, strFormat, arglist );
	va_end( arglist );

	// Output to the Debugger.
	OutputDebugStringA( g_strMsgString );
}