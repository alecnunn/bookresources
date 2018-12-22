//--------------------------------------------------------------------------------------
// File: GPUTerrain.cpp
//
// Copyright (C) 2005 - Harald Vistnes (harald@vistnes.org)
// All rights reserved worldwide.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (C) Harald Vistnes, 2005"
//--------------------------------------------------------------------------------------

#include "dxstdafx.h"
#include "resource.h"
#include "Camera.h"
#include "Terrain.h"

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN	1
#define IDC_TOGGLEREF			3
#define IDC_CHANGEDEVICE		4
#define IDC_ENABLE_PRESHADER	5
#define IDC_NUM_LIGHTS			6
#define IDC_NUM_LIGHTS_STATIC	7
#define IDC_ACTIVE_LIGHT		8
#define IDC_LIGHT_SCALE			9
#define IDC_LIGHT_SCALE_STATIC	10
#define IDC_TOGGLENORMAL		11
#define IDC_SLIDERLOD			12
#define IDC_SLIDERLODTEXT		13
#define IDC_TOGGLEWIREFRAME		14
#define IDC_TOGGLECULLING		15
#define IDC_BLOCKSIZE			17

ID3DXFont*					g_pFont = NULL;         // Font for drawing text
ID3DXSprite*				g_pSprite = NULL;       // Sprite for batching draw text calls
CDXUTDialogResourceManager	g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg				g_SettingsDlg;          // Device settings dialog
CDXUTDialog					g_HUD;                  // manages the 3D UI
Terrain						g_Terrain;
D3DCAPS9					g_Caps;
bool						g_bShowInfo = true;

//--------------------------------------------------------------------------------------
// Handles the keyboard events
//--------------------------------------------------------------------------------------

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if( bKeyDown )
	{
		switch( nChar )
		{
		case VK_F1:
			g_bShowInfo = !g_bShowInfo;
			break;
		case VK_F10:
			g_Terrain.CreateScreenShot();
			break;
		case VK_UP:
			g_Camera.MoveForward();
			break;
		case VK_DOWN:
			g_Camera.MoveBackward();
			break;
		case VK_LEFT:
			g_Camera.StrafeLeft();
			break; 
		case VK_RIGHT:
			g_Camera.StrafeRight();
			break;
		case 'A':
			g_Camera.MoveUp();
			break;
		case 'Z':
			g_Camera.MoveDown();
			break;
		case 'W':
			g_Terrain.ToggleWireframe();
			break;
		}
	}
}

//--------------------------------------------------------------------------------------
// Handles the keyboard events
//--------------------------------------------------------------------------------------

void CALLBACK MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext)
{
	static int xPrev = xPos; 
	static int yPrev = yPos; 

	if (bLeftButtonDown)
	{
		g_Camera.SetViewByMouse(-xPos+xPrev, yPos-yPrev);
	}
	xPrev = xPos;
	yPrev = yPos;
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{   
	switch( nControlID )
	{
	case IDC_TOGGLEFULLSCREEN: 
		DXUTToggleFullScreen(); 
		break;
	case IDC_TOGGLEREF:        
		DXUTToggleREF(); 
		break;
	case IDC_CHANGEDEVICE:     
		g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); 
		break;
	case IDC_TOGGLEWIREFRAME:
		g_Terrain.ToggleWireframe();
		break;
	case IDC_TOGGLENORMAL:		
		g_Terrain.ToggleNormalConstruction(); 
		break;
	case IDC_TOGGLECULLING:
		g_Terrain.ToggleCulling();
		break;
	case IDC_SLIDERLOD:
		if ( nEvent == EVENT_SLIDER_VALUE_CHANGED ) {
			CDXUTSlider *pSlider = (CDXUTSlider *)pControl;
			float fLOD = (float)pSlider->GetValue();
			g_Terrain.SetLOD(fLOD);
			CDXUTStatic *pStatic = g_HUD.GetStatic( IDC_SLIDERLODTEXT );
			if (pStatic) {
				WCHAR wszText[128];
				StringCchPrintf( wszText, 128, L"LOD factor: %0.2f", fLOD);
				pStatic->SetText( wszText );
			}
		}
		break;
	case IDC_BLOCKSIZE:
		{
			int iBlockSize = g_Terrain.GetBlockSize();
			iBlockSize = (iBlockSize-1) * 2 + 1;
			if (iBlockSize > 65) iBlockSize = 9;
			g_Terrain.SetBlockSize(iBlockSize);
		}
		break;
	}
    
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText( IDirect3DDevice9 *pd3dDevice)
{
	if (!g_bShowInfo) {
		return;
	}

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

	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	txtHelper.DrawFormattedTextLine( L"Block size: %dx%d", g_Terrain.GetBlockSize(), g_Terrain.GetBlockSize());
	txtHelper.DrawFormattedTextLine( L"Tris/sec: %.1fM", g_Terrain.GetNumTrianglesPerSecond()*1.0e-6f );

	txtHelper.DrawFormattedTextLine( L"Frustum culling %s - %d blocks culled", g_Terrain.IsCulling() ? L"enabled" : L"disabled", g_Terrain.GetNumCulled());

	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	txtHelper.DrawTextLine( L"Press F1 to hide text and buttons" );

	txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	// Initialize dialogs
	g_SettingsDlg.Init( &g_DialogResourceManager );
	g_HUD.Init( &g_DialogResourceManager );

	g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
	g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 135, 22 );
	g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 135, 22, VK_F2 );
	g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 135, 22 );
	g_HUD.AddButton( IDC_TOGGLEWIREFRAME, L"Toggle wireframe (F4)", 35, iY += 24, 135, 22, VK_F4 );
	g_HUD.AddButton( IDC_TOGGLENORMAL, L"Normal construction (F5)", 35, iY += 24, 135, 22, VK_F5 );
	g_HUD.AddButton( IDC_TOGGLECULLING, L"Toggle frustum culling", 35, iY += 24, 135, 22, VK_F6 );
	g_HUD.AddButton( IDC_BLOCKSIZE, L"Change block size", 35, iY += 24, 135, 22 );
	g_HUD.AddStatic( IDC_SLIDERLODTEXT, L"LOD factor: xxx", 35, iY += 24, 125, 16 );
	g_HUD.AddSlider( IDC_SLIDERLOD, 35, iY += 17, 125, 22, 1, 200 );
}

//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	// Typically want to skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3DObject(); 
	if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
				AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
				D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
	{
		return false;
	}
	return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	// initialize the terrain
	if (FAILED(g_Terrain.Initialize(pd3dDevice, 17))) {
		return E_FAIL;
	}

	V_RETURN( g_DialogResourceManager.OnCreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnCreateDevice( pd3dDevice ) );

	// Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &g_pFont ) );

	// update the LOD slider to the default value set in Terrain
	float fLOD = g_Terrain.GetLOD();
	CDXUTSlider *pSlider = g_HUD.GetSlider(IDC_SLIDERLOD);
	if (pSlider) {
		pSlider->SetValue(int(fLOD));
	}
	CDXUTStatic *pStatic = g_HUD.GetStatic( IDC_SLIDERLODTEXT );
	if (pStatic) {
		WCHAR wszText[128];
		StringCchPrintf( wszText, 128, L"LOD factor: %0.2f", fLOD);
		pStatic->SetText( wszText );
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	V_RETURN( g_DialogResourceManager.OnResetDevice() );
	V_RETURN( g_SettingsDlg.OnResetDevice() );

	if (g_pFont) {
		V_RETURN( g_pFont->OnResetDevice() );
	}

	// Create a sprite to help batch calls when drawing many lines of text
	V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite ) );

	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	D3DXMatrixPerspectiveFovLH(&g_Camera.GetProjectionMatrix(), D3DXToRadian(60.0f), fAspectRatio, ZNEAR, ZFAR);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_Camera.GetProjectionMatrix());

	if (FAILED(g_Terrain.OnResetDevice(pd3dDevice))) {
		return E_FAIL;
	}

	g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
	g_HUD.SetSize( 170, 170 );
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_Terrain.OnFrameMove(fTime);
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // If the settings dialog is being shown, then
    // render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

	// Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pd3dDevice->LightEnable(0, FALSE);

		g_Terrain.OnRender();

		if (g_bShowInfo) {
			g_HUD.OnRender( fElapsedTime ); 
		}

		RenderText(pd3dDevice);
		V( pd3dDevice->EndScene() );
    }

}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
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

	return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	g_DialogResourceManager.OnLostDevice();
	g_SettingsDlg.OnLostDevice();
	if (g_pFont) {
		g_pFont->OnLostDevice();
	}
	if (g_pSprite) {
		g_pSprite->OnLostDevice();
	}
	g_Terrain.OnLostDevice();

	SAFE_RELEASE(g_pSprite);
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	g_DialogResourceManager.OnDestroyDevice();
	g_SettingsDlg.OnDestroyDevice();
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pSprite);
	g_Terrain.OnDestroyDevice();
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
	DXUTSetCallbackKeyboard( KeyboardProc );
	DXUTSetCallbackMouse(MouseProc, true, 0);   
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackFrameRender( OnFrameRender );
	DXUTSetCallbackFrameMove( OnFrameMove );
	InitApp();

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"GPU Terrain" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

    // Start the render loop
    DXUTMainLoop();

    return DXUTGetExitCode();
}
