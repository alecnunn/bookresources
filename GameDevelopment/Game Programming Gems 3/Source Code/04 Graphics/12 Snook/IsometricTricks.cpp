/****************************************************************************************\

	IsometricTricks.cpp

	IsometricTricks component interface for a basic terrain manager.

	Part of the Gems III sample code library.
	Created 12/27/2001 by Greg Snook

    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#define STRICT
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <commdlg.h>
#include <d3dx8.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "resource.h"
#include "DualMultiPlaneMethod.h"
#include "ImageWarpMethod.h"
#include "ImageBlendMethod.h"

#define MOVE_SPEED 10.0f

class IsometricTricks : public CD3DApplication
{
public:

	// Data Types & Constants...
	enum METHOD
	{
		METHOD_A = 0,
		METHOD_B,
		METHOD_C,
		ALL_METHODS,
	};

	// Creators...

	IsometricTricks();
	~IsometricTricks(){};

	// Operators...

	// Mutators...
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();

    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Accessors...

private:

	// Data...
    CD3DFont*           m_pFont;              // Font for outputting frame stats
	D3DXVECTOR3			m_WorldOffset;

	DualMultiPlaneMethod	m_MethodA;
	ImageWarpMethod			m_MethodB;
	ImageBlendMethod		m_MethodC;

	METHOD				m_MethodToShow;
	bool				m_ShowWireframe;

	D3DXMATRIX			m_ViewMatrix;
	D3DXMATRIX			m_ProjMatrix;

	// Private Functions...
	HRESULT RenderText();

	// Nonexistant Functions...

	IsometricTricks( const IsometricTricks& Src);
	IsometricTricks& operator=( const IsometricTricks& Src);

};

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    IsometricTricks d3dApp;

    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: IsometricTricks()
// Desc: Constructor
//-----------------------------------------------------------------------------
IsometricTricks::IsometricTricks()
:m_MethodToShow(METHOD_A)
,m_ShowWireframe(false)
{
    // Override base class members
    m_strWindowTitle     = _T("GEMS III: Isometric Tricks");
    m_bUseDepthBuffer    = TRUE;
    m_bShowCursorWhenFullscreen = TRUE;

	m_WorldOffset = D3DXVECTOR3(0.0f,0.0f,0.0f);

    // Initialize member variables
    m_pFont              = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );

}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::OneTimeSceneInit()
{
    // Set cursor to indicate that user can move the object with the mouse
#ifdef _WIN64
    SetClassLongPtr( m_hWnd, GCLP_HCURSOR, (LONG_PTR)LoadCursor( NULL, IDC_SIZEALL ) );
#else
    SetClassLong( m_hWnd, GCL_HCURSOR, (LONG)LoadCursor( NULL, IDC_SIZEALL ) );
#endif

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::FrameMove()
{
 	// process keyboard input
    bool bMoveUp			= ( m_bActive && (GetAsyncKeyState( 'A' )    & 0x8000) == 0x8000 );
    bool bMoveDown			= ( m_bActive && (GetAsyncKeyState( 'Z' )  & 0x8000) == 0x8000 );
    bool bMoveForward		= ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    bool bMoveBack			= ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    bool bMoveLeft			= ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    bool bMoveRight			= ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );

    if( bMoveLeft) m_WorldOffset.x += m_fElapsedTime*MOVE_SPEED;
    if( bMoveRight) m_WorldOffset.x -= m_fElapsedTime*MOVE_SPEED;
    if( bMoveForward) m_WorldOffset.y += m_fElapsedTime*MOVE_SPEED;
    if( bMoveBack) m_WorldOffset.y -= m_fElapsedTime*MOVE_SPEED;
    if( bMoveUp) m_WorldOffset.z += m_fElapsedTime*MOVE_SPEED;
    if( bMoveDown) m_WorldOffset.z -= m_fElapsedTime*MOVE_SPEED;

	D3DXVECTOR3 camera_pos;

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -10.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	vFromPt += m_WorldOffset;
	vLookatPt += m_WorldOffset;

    D3DXMatrixLookAtLH( &m_ViewMatrix, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_ViewMatrix );


    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::Render()
{
    // Clear the scene
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0x00000000 );

    // Draw scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		D3DXMATRIX worldMatrix;
		D3DXMATRIX idMatrix;

		D3DXMatrixIdentity(&idMatrix);

		// setup the world transform
		D3DXMatrixTranslation(&worldMatrix, 0.0f, 0.0f, 0.0f);
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &worldMatrix );

		if (m_ShowWireframe)
		{
			m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		}
		else
		{
			m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}

		switch (m_MethodToShow)
		{
			case METHOD_A:
				m_MethodA.Render(worldMatrix, m_ViewMatrix, m_ProjMatrix, m_ShowWireframe);
			break;

			case METHOD_B:
				m_MethodB.Render(worldMatrix, m_ViewMatrix, m_ProjMatrix, m_ShowWireframe);
			break;

			case METHOD_C:
				m_MethodC.Render(worldMatrix, m_ViewMatrix, m_ProjMatrix, m_ShowWireframe);
			break;

			case ALL_METHODS:
				{
					float x;
					for (x = -1.5f; x<= 1.5f; x+= 0.75f)
					{
						D3DXMatrixTranslation(&worldMatrix, x, 1.0f, 0.0f);
						m_pd3dDevice->SetTransform( D3DTS_WORLD,  &worldMatrix );
						m_MethodA.Render(worldMatrix, m_ViewMatrix, m_ProjMatrix, m_ShowWireframe);
					}
					for (x = -1.5f; x<= 1.5f; x+= 0.75f)
					{
						D3DXMatrixTranslation(&worldMatrix, x, 0.0f, 0.0f);
						m_pd3dDevice->SetTransform( D3DTS_WORLD,  &worldMatrix );
						m_MethodB.Render(worldMatrix, m_ViewMatrix, m_ProjMatrix, m_ShowWireframe);
					}
					for (x = -1.5f; x<= 1.5f; x+= 0.75f)
					{
						D3DXMatrixTranslation(&worldMatrix, x, -1.0f, 0.0f);
						m_pd3dDevice->SetTransform( D3DTS_WORLD,  &worldMatrix );
						m_MethodC.Render(worldMatrix, m_ViewMatrix, m_ProjMatrix, m_ShowWireframe);
					}
				}
			break;
		};

		RenderText();
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 

    lstrcpy( szMsg, TEXT("Use arrow keys to move camera, A & Z to zoom") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	switch (m_MethodToShow)
	{
		case METHOD_A:
			lstrcpy( szMsg, TEXT("Method 1: Dual Elevation Map") );
			fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
		break;

		case METHOD_B:
			lstrcpy( szMsg, TEXT("Method 2: Procedural Relief Texture") );
			fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
		break;

		case METHOD_C:
			lstrcpy( szMsg, TEXT("Method 3: Pre-Warped, Interlaced Texture") );
			fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
		break;

		case ALL_METHODS:
			lstrcpy( szMsg, TEXT("All Methods") );
			fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
		break;
	};

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::InitDeviceObjects()
{
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	m_MethodA.InitDeviceObjects( m_pd3dDevice );
	m_MethodB.InitDeviceObjects( m_pd3dDevice );
	m_MethodC.InitDeviceObjects( m_pd3dDevice );

	// load our sample textures and create geometry

    return D3D_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::RestoreDeviceObjects()
{
    // Restore the font
    m_pFont->RestoreDeviceObjects();

	m_MethodA.RestoreDeviceObjects();
	m_MethodB.RestoreDeviceObjects();
	m_MethodC.RestoreDeviceObjects();

    // Setup render state
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    // Setup the light
    D3DLIGHT8 light;
    light.Type         = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r    = light.Diffuse.g  = light.Diffuse.b  = 1.0f;
    light.Specular.r   = light.Specular.g = light.Specular.b = 0.0f;
    light.Ambient.r    = light.Ambient.g  = light.Ambient.b  = 0.3f;
    light.Position     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3( 0.3f, -1.0f, 1.0f ) );
    light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
    light.Range        = sqrtf(FLT_MAX);
    m_pd3dDevice->SetLight(0, &light );
    m_pd3dDevice->LightEnable(0, TRUE );

    FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;

	D3DXMatrixPerspectiveFovLH( &m_ProjMatrix, D3DX_PI/10.0f, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_ProjMatrix );


    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();

	m_MethodA.InvalidateDeviceObjects();
	m_MethodB.InvalidateDeviceObjects();
	m_MethodC.InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();

	m_MethodA.DeleteDeviceObjects();
	m_MethodB.DeleteDeviceObjects();
	m_MethodC.DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT IsometricTricks::FinalCleanup()
{
    SAFE_DELETE( m_pFont );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT IsometricTricks::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
	HMENU hMenu = GetMenu(hWnd);

	// Trap the context menu
    if( WM_CONTEXTMENU==uMsg )
        return 0;

	if (WM_COMMAND == uMsg && hMenu)
	{
        switch( LOWORD(wParam) )
        {
            case IDM_METHOD_A:
                m_MethodToShow = METHOD_A;
                CheckMenuItem(hMenu, IDM_METHOD_A, MF_BYCOMMAND|MF_CHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_B, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_C, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_ALL, MF_BYCOMMAND|MF_UNCHECKED);
                break;
            case IDM_METHOD_B:
                m_MethodToShow = METHOD_B;
                CheckMenuItem(hMenu, IDM_METHOD_A, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_B, MF_BYCOMMAND|MF_CHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_C, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_ALL, MF_BYCOMMAND|MF_UNCHECKED);
               break;
            case IDM_METHOD_C:
                m_MethodToShow = METHOD_C;
                CheckMenuItem(hMenu, IDM_METHOD_A, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_B, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_C, MF_BYCOMMAND|MF_CHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_ALL, MF_BYCOMMAND|MF_UNCHECKED);
                break;
            case IDM_METHOD_ALL:
                m_MethodToShow = ALL_METHODS;
                CheckMenuItem(hMenu, IDM_METHOD_A, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_B, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_C, MF_BYCOMMAND|MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_METHOD_ALL, MF_BYCOMMAND|MF_CHECKED);
                break;
            case IDM_WIREFRAME:
                // Toggle wireframe
                m_ShowWireframe = !m_ShowWireframe;
                CheckMenuItem(hMenu, IDM_WIREFRAME, MF_BYCOMMAND|(m_ShowWireframe ? MF_CHECKED:MF_UNCHECKED));
                break;
		}
	}

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}

//- End of IsometricTricks -----------------------------------------------------------------
//****************************************************************************************

