//-----------------------------------------------------------------------------
// File: App.cpp
//
// Desc: DirectX MFC dialog application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
//#define STRICT
#include "stdafx.h"
#include "App.h"

#include "Scene.h"
#include "Input.h"
#include "Camera.h"
#include "Sun.h"
#include "Atmosphere.h"
#include "Sky.h"
#include "Anim.h"
#include "State.h"
#include "Movie.h"

//-----------------------------------------------------------------------------
// Application globals
//-----------------------------------------------------------------------------
TCHAR*          g_strAppTitle       = _T( "Real-Time Atmospheric Light Scattering" );
CApp            g_App;
HINSTANCE       g_hInst = NULL;
CAppForm*       g_AppFormView = NULL;
LPDIRECT3DDEVICE8	g_Device = NULL;
HWND			g_hWnd = NULL;
int g_ScatterVS = 0;
float g_fSlowFactor = 1.0f;
char *readFileName = "Movie.dat";
char *dumpFileName = "Dump.dat";


//-----------------------------------------------------------------------------
// The MFC macros are all listed here
//-----------------------------------------------------------------------------
IMPLEMENT_DYNCREATE( CAppDoc,      CDocument )
IMPLEMENT_DYNCREATE( CAppFrameWnd, CFrameWnd )
IMPLEMENT_DYNCREATE( CAppForm,     CFormView )




BEGIN_MESSAGE_MAP( CApp, CWinApp )
    //{{AFX_MSG_MAP(CApp)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




BEGIN_MESSAGE_MAP( CAppForm, CFormView )
    //{{AFX_MSG_MAP(CAppForm)
    ON_COMMAND(IDC_VIEWFULLSCREEN, OnToggleFullScreen )
    ON_BN_CLICKED(IDC_CHANGEDEVICE, OnChangeDevice)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SUN_THETA, OnCustomdrawSliderSunTheta)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SUN_PHI, OnCustomdrawSliderSunPhi)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SUN_INTENSITY, OnCustomdrawSliderSunIntensity)
	ON_EN_UPDATE(IDC_EDIT_SUN_INTENSITY_SCALE, OnUpdateEditSunIntensityScale)
	ON_EN_UPDATE(IDC_EDIT_BETA_RAY_SCALE, OnUpdateEditBetaRayScale)
	ON_EN_UPDATE(IDC_EDIT_BETA_MIE_SCALE, OnUpdateEditBetaMieScale)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BETA_MIE, OnCustomdrawSliderBetaMie)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BETA_RAY, OnCustomdrawSliderBetaRay)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HG_G, OnCustomdrawSliderHgG)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MULT_EXTINCTION, OnCustomdrawSliderMultExtinction)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MULT_INSCATTERING, OnCustomdrawSliderMultInscattering)
	ON_BN_CLICKED(IDC_CHECK_MULT_EXTINCTION, OnCheckMultExtinction)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TIME, OnCustomdrawSliderTime)
	ON_COMMAND(ID_PLAY, OnPlay)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()




BEGIN_MESSAGE_MAP(CAppDoc, CDocument)
    //{{AFX_MSG_MAP(CAppDoc)
            // NOTE - the ClassWizard will add and remove mapping macros here.
            //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




BEGIN_MESSAGE_MAP(CAppFrameWnd, CFrameWnd)
    //{{AFX_MSG_MAP(CAppFrameWnd)
    ON_COMMAND(IDM_CHANGEDEVICE, OnChangeDevice)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CAppForm *GetApp()
{
	return g_AppFormView;
}

//-----------------------------------------------------------------------------
// Name: CAppForm()
// Desc: Constructor for the dialog resource form
//-----------------------------------------------------------------------------
CAppForm::CAppForm()
         :CFormView( IDD_FORMVIEW )
{
    //{{AFX_DATA_INIT(CAppForm)

	m_SliderSunIntensity = 100;
	m_SliderSunTheta = 0;
	m_SliderSunPhi = 0;
	m_SliderBeta = 100;
	m_EditSunIntensityScale = 100;
	m_EditBetaRayScale = 1.0f;
	m_EditBetaMieScale = 1.0f;
	m_SliderBetaMie = 1;
	m_SliderBetaRay = 100;
	m_SliderHGg = 80;
	m_SliderMultInscattering = 100;
	m_CheckMultExtinction = true;
	m_SliderTime = 0;	// Zenith.
	m_bDemo = FALSE;

	//}}AFX_DATA_INIT

    g_AppFormView          = this;
    m_hwndRenderWindow     = NULL;
    m_hwndRenderFullScreen = NULL;
    m_hWndTopLevelParent   = NULL;

    // Override some CD3DApplication defaults:
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 500;
    m_strWindowTitle            = TEXT( "Demo" );
    m_bUseDepthBuffer           = TRUE;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

	m_pScene = NULL;
	m_pCamera = NULL;
	m_pInput = NULL;
	m_pSun = NULL;
	m_pAnim =NULL;
	m_pMovie = NULL;
	m_pCurKeyState = NULL;
	m_bPlayMode = false;			// This mode is used to play some data files already recorded. 


    m_dwMinDepthBits = 24;    // Minimum number of bits needed in depth buffer


}




//-----------------------------------------------------------------------------
// Name: CAppForm::OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CAppForm::OneTimeSceneInit()
{
    // TODO: perform one time initialization

    m_bLoadingApp = FALSE;


	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CAppForm::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    BOOL bCapsAcceptable = TRUE;;
    
	// TODO: Perform checks to see if these display caps are acceptable.
	if( D3DSHADER_VERSION_MAJOR( pCaps->VertexShaderVersion ) < 1 )
		return E_FAIL;
	if( D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
		return E_FAIL;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CAppForm::InitDeviceObjects()
{
	// Set global variables.
	g_Device = m_pd3dDevice;
	g_hWnd = m_hWndTopLevelParent;

	// Init other entities of app.
	m_pScene = new Scene;
	m_pInput = new Input;
	m_pCamera = new Camera;
	m_pSun = new Sun;
	m_pAtmosphere = new Atmosphere;
	m_pMovie = new Movie;
    
    
	// TODO: create device objects

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );



    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CAppForm::RestoreDeviceObjects()
{
    // Restore the font
    m_pFont->RestoreDeviceObjects();



    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CAppForm::FrameMove()
{
    // TODO: update world
	State *interpolatedState;
	if (m_bPlayMode)
	{
		interpolatedState = m_pAnim->GetNextFrame();
		while (interpolatedState == NULL)
		{
			m_pMovie->AdvanceAnim();
			m_pAnim = m_pMovie->GetCurrentAnim();

			interpolatedState = m_pAnim->GetNextFrame();
		}

		m_pCamera = interpolatedState->GetCamera();
		m_pSun = interpolatedState->GetSun();
		m_pAtmosphere = interpolatedState->GetAtmosphere();

		UpdateUI();

	}
	else
	{
	   // Update user input 
		UpdateInput();
	}


	return S_OK;

}





//-----------------------------------------------------------------------------
// Name: CAppForm::Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CAppForm::Render()
{
	// Start Timer.
	DXUtil_Timer( TIMER_GETELAPSEDTIME  );

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );


    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
         // TODO: render world
		m_pScene->Render();
      
        // Render stats and help text  
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
	}


	// End timer
	FLOAT fTimePerFrame = DXUtil_Timer( TIMER_GETELAPSEDTIME  ) * 1000;

	float fDesiredTimePerFrame = 1000.0/40.0; // in ms (for 40 frames per sec)
	if (fTimePerFrame < fDesiredTimePerFrame) 
		Sleep(fDesiredTimePerFrame-fTimePerFrame);
 
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CAppForm::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 20.0f; 

/*    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
*/
    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DoDataExchange()
// Desc: DDX/DDV support
//-----------------------------------------------------------------------------
void CAppForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppForm)
	DDX_Slider(pDX, IDC_SLIDER_SUN_INTENSITY, m_SliderSunIntensity);
	DDX_Slider(pDX, IDC_SLIDER_BETA_MIE, m_SliderBetaMie);
	DDX_Slider(pDX, IDC_SLIDER_BETA_RAY, m_SliderBetaRay);
	DDX_Slider(pDX, IDC_SLIDER_HG_G, m_SliderHGg);
	DDX_Slider(pDX, IDC_SLIDER_MULT_INSCATTERING, m_SliderMultInscattering);
	DDX_Slider(pDX, IDC_SLIDER_TIME, m_SliderTime);
	//}}AFX_DATA_MAP
	
}




//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CAppForm::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    return CFormView ::WindowProc(message, wParam, lParam);
}




//-----------------------------------------------------------------------------
// Name: OnChangeDevice()
// Desc: Needed to enable dlg menu item 
//-----------------------------------------------------------------------------
void CAppFrameWnd::OnChangeDevice() 
{
    g_AppFormView->OnChangeDevice();
}

void CAppForm::OnPlay() 
{
	if (m_bPlayMode) return;
	// Delete existing created objects.
	// these just now point to objects created from reading
	SAFE_DELETE(m_pSun);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pAtmosphere);
	SAFE_DELETE(m_pAnim);



	m_fpRead = fopen(readFileName, "r");
	ReadMovie();
	fclose(m_fpRead);

	m_bPlayMode = true;

	// Set up drawing for first frame
	m_pAnim = m_pMovie->GetCurrentAnim();



}



//-----------------------------------------------------------------------------
// Name: CAppForm::InvalidateDeviceObjcts()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CAppForm::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CAppForm::DeleteDeviceObjects()
{
    // TODO: Cleanup any objects created in InitDeviceObjects()
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CAppForm::FinalCleanup()
{
    // TODO: Perform any final cleanup needed
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );
	SAFE_DELETE( m_pScene );
	SAFE_DELETE (m_pInput);
	SAFE_DELETE (m_pMovie);
	if (!m_bPlayMode)
	{
		SAFE_DELETE (m_pCamera);
		SAFE_DELETE (m_pSun);
		SAFE_DELETE (m_pAtmosphere);
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitInstance()
// Desc: This is the main entry point for the application. The MFC window stuff
//       is initialized here. See also the main initialization routine for the
//       CAppForm class, which is called indirectly from here.
//-----------------------------------------------------------------------------
BOOL CApp::InitInstance()
{

    // Asscociate the MFC app with the frame window and doc/view classes
    AddDocTemplate( new CSingleDocTemplate( IDR_MAINFRAME,
                                            RUNTIME_CLASS(CAppDoc),
                                            RUNTIME_CLASS(CAppFrameWnd),
                                            RUNTIME_CLASS(CAppForm) ) );

    // Dispatch commands specified on the command line (req'd by MFC). This
    // also initializes the the CAppDoc, CAppFrameWnd, and CAppForm classes.
    CCommandLineInfo cmdInfo;
    ParseCommandLine( cmdInfo );
    if( !ProcessShellCommand( cmdInfo ) )
        return FALSE;

    if( !g_AppFormView->IsReady() )
        return FALSE;

    g_AppFormView->GetParentFrame()->RecalcLayout();
    g_AppFormView->ResizeParentToFit( FALSE ); 
    
    m_pMainWnd->SetWindowText( g_strAppTitle );
    m_pMainWnd->UpdateWindow();

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: LoadFrame()
// Desc: Uses idle time to render the 3D scene.
//-----------------------------------------------------------------------------
BOOL CAppFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
    BOOL bResult = CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);

    LoadAccelTable( MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

    return bResult;
}




//-----------------------------------------------------------------------------
// Name: OnIdle()
// Desc: Uses idle time to render the 3D scene.
//-----------------------------------------------------------------------------
BOOL CApp::OnIdle( LONG )
{
    // Do not render if the app is minimized
    if( m_pMainWnd->IsIconic() )
        return FALSE;

    TCHAR strStatsPrev[200];

    lstrcpy(strStatsPrev, g_AppFormView->PstrFrameStats());

    // Update and render a frame
    if( g_AppFormView->IsReady() )
    {
        g_AppFormView->CheckForLostFullscreen();
        g_AppFormView->RenderScene();
    }

    // Keep requesting more idle time
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: PreCreateWindow()
// Desc: Change the window style (so it cannot maximize or be sized) before
//       the main frame window is created.
//-----------------------------------------------------------------------------
BOOL CAppFrameWnd::PreCreateWindow( CREATESTRUCT& cs )
{
    cs.style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;

    return CFrameWnd::PreCreateWindow( cs );
}




//-----------------------------------------------------------------------------
// Name: ~CAppForm()
// Desc: Destructor for the dialog resource form. Shuts down the app
//-----------------------------------------------------------------------------
CAppForm::~CAppForm()
{
    Cleanup3DEnvironment();
}




//-----------------------------------------------------------------------------
// Name: OnToggleFullScreen()
// Desc: Called when user toggles the fullscreen mode
//-----------------------------------------------------------------------------
void CAppForm::OnToggleFullScreen()
{
    ToggleFullscreen();
}




//-----------------------------------------------------------------------------
// Name: OnChangeDevice()
// Desc: Use hit the "Change Device.." button. Display the dialog for the user
//       to select a new device/mode, and call Change3DEnvironment to
//       use the new device/mode.
//-----------------------------------------------------------------------------
VOID CAppForm::OnChangeDevice()
{
	return; 	// No Change Device supported

    Pause(TRUE);

    UserSelectNewDevice();

    // Update UI
    UpdateUIForDeviceCapabilites();

    Pause(FALSE);
}




//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Adjusts the window properties for windowed or fullscreen mode
//-----------------------------------------------------------------------------
HRESULT CAppForm::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        ::ShowWindow( m_hwndRenderFullScreen, SW_HIDE );
        CD3DApplication::m_hWnd = m_hwndRenderWindow;
    }
    else
    {
        if( ::IsIconic( m_hwndRenderFullScreen ) )
            ::ShowWindow( m_hwndRenderFullScreen, SW_RESTORE );
        ::ShowWindow( m_hwndRenderFullScreen, SW_SHOW );
        CD3DApplication::m_hWnd = m_hwndRenderFullScreen;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    if( msg == WM_CLOSE )
    {
        // User wants to exit, so go back to windowed mode and exit for real
        g_AppFormView->OnToggleFullScreen();
        g_App.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }
    else if( msg == WM_SETCURSOR )
    {
        SetCursor( NULL );
    }
    else if( msg == WM_KEYUP && wParam == VK_ESCAPE )
    {
        // User wants to leave fullscreen mode
        g_AppFormView->OnToggleFullScreen();
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}



//-----------------------------------------------------------------------------
// Name: CheckForLostFullscreen()
// Desc: If fullscreen and device was lost (probably due to alt-tab), 
//       automatically switch to windowed mode
//-----------------------------------------------------------------------------
HRESULT CAppForm::CheckForLostFullscreen()
{
    HRESULT hr;

    if( m_bWindowed )
        return S_OK;

    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
        ForceWindowed();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateUIForDeviceCapabilites()
// Desc: Whenever we get a new device, call this function to enable/disable the
//       appropiate UI controls to match the device's capabilities.
//-----------------------------------------------------------------------------
VOID CAppForm::UpdateUIForDeviceCapabilites()
{
    // TODO: Check the capabilities of the device and update the UI as needed
    DWORD dwCaps = m_d3dCaps.RasterCaps;
}




//-----------------------------------------------------------------------------
// Name: OnInitialUpdate()
// Desc: When the AppForm object is created, this function is called to
//       initialize it. Here we getting access ptrs to some of the controls,
//       and setting the initial state of some of them as well.
//-----------------------------------------------------------------------------
VOID CAppForm::OnInitialUpdate()
{
    // Update the UI
    CFormView::OnInitialUpdate();

    // Get the top level parent hwnd
    m_hWndTopLevelParent = GetTopLevelParent()->GetSafeHwnd();

    // Save static reference to the render window
    m_hwndRenderWindow = GetDlgItem(IDC_RENDERVIEW)->GetSafeHwnd();

    // Register a class for a fullscreen window
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
                          NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                          _T("Fullscreen Window") };
    RegisterClass( &wndClass );

    // We create the fullscreen window (not visible) at startup, so it can
    // be the focus window.  The focus window can only be set at CreateDevice
    // time, not in a Reset, so ToggleFullscreen wouldn't work unless we have
    // already set up the fullscreen focus window.
    m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
                                           WS_POPUP, CW_USEDEFAULT,
                                           CW_USEDEFAULT, 100, 100,
                                           m_hWndTopLevelParent, 0L, NULL, 0L );

    // Note that for the MFC samples, the device window and focus window
    // are not the same.
    CD3DApplication::m_hWnd = m_hwndRenderWindow;
    CD3DApplication::m_hWndFocus = m_hwndRenderFullScreen;
    CD3DApplication::Create( AfxGetInstanceHandle() );

    // TODO: Update the UI as needed
}


void CAppForm::UpdateInput()
{

	CameraMovements cm;
	m_bHasFocus = ( m_hWndTopLevelParent == GetForegroundWindow()->GetSafeHwnd() );
  
	if  ( m_bHasFocus && (GetAsyncKeyState( VK_HOME )    & 0x8000) == 0x8000 )
		cm = eCameraMoveUp;
	else if ( m_bHasFocus && (GetAsyncKeyState( VK_END )    & 0x8000) == 0x8000 )
		cm = eCameraMoveDown;

	else if ( m_bHasFocus && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 )
		cm = eCameraMoveForward;
    else if  ( m_bHasFocus && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 )
		cm = eCameraMoveBackward;

    else if ( m_bHasFocus && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 )
		cm = eCameraYawLeft;
    else if ( m_bHasFocus && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 )
		cm = eCameraYawRight;

    else if  ( m_bHasFocus && (GetAsyncKeyState( VK_NEXT)  & 0x8000) == 0x8000 )
		cm = eCameraPitchUp;
    else if ( m_bHasFocus && (GetAsyncKeyState( VK_PRIOR ) & 0x8000) == 0x8000 )
		cm = eCameraPitchDown;
    else if  ( m_bHasFocus && (GetAsyncKeyState( VK_F7)  & 0x8000) == 0x8000 )
		cm = eCameraMoveLeft;
    else if ( m_bHasFocus && (GetAsyncKeyState( VK_F8 ) & 0x8000) == 0x8000 )
		cm = eCameraMoveRight;

    else if  ( m_bHasFocus && (GetAsyncKeyState( VK_F9)  & 0x8000) == 0x8000 )
		cm = eCameraRollLeft;
    else if ( m_bHasFocus && (GetAsyncKeyState( VK_F11 ) & 0x8000) == 0x8000 )
		cm = eCameraRollRight;
	else if ( m_bHasFocus && (GetAsyncKeyState( VK_F6 ) & 0x8000) == 0x8000 )
	{
		if (m_pCamera) m_pCamera->Faster();
		return;
	}
	else if ( m_bHasFocus && (GetAsyncKeyState( VK_F5 ) & 0x8000) == 0x8000 )
	{
		if (m_pCamera) m_pCamera->Slower();
		return;
	}
	
	if (m_pCamera) m_pCamera->Update(cm);


}



float CAppForm::UIGetExtinctionMultiplier()
{
	return (float)(m_CheckMultExtinction);
}


float CAppForm::UIGetInscatteringMultiplier()
{
	return m_SliderMultInscattering/100.0f;
}

float CAppForm::UIGetBetaMieMultiplier()
{
	return m_SliderBetaMie/100.0f;
}

float CAppForm::UIGetBetaMieScale()
{
	return m_EditBetaMieScale;
}

float CAppForm::UIGetBetaRayMultiplier()
{
	return m_SliderBetaRay/100.0f;
}

float CAppForm::UIGetBetaRayScale()
{
	return m_EditBetaRayScale;
}


float CAppForm::UIGetHGg()
{
	return m_SliderHGg/100.0f;
}

float CAppForm::UIGetSunIntensity()
{
	return m_SliderSunIntensity/100.0f;
}

float CAppForm::UIGetSunIntensityScale()
{
	return m_EditSunIntensityScale;
}

float CAppForm::UIGetSunTheta()
{
	return m_SliderSunTheta/100.0f*D3DXToRadian(90);
}

float CAppForm::UIGetSunPhi()
{
	return	m_SliderSunPhi/100.0f*D3DXToRadian(360);
}

float CAppForm::UIGetTime()
{
	return	m_SliderTime/100.0f;
}


void CAppForm::OnCustomdrawSliderSunTheta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pSun) m_pSun->SetSunThetaPhi(UIGetSunTheta(), UIGetSunPhi()) ;
	
	*pResult = 0;
}

void CAppForm::OnCustomdrawSliderSunPhi(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pSun) m_pSun->SetSunThetaPhi(UIGetSunTheta(), UIGetSunPhi()) ;
	
	*pResult = 0;
}

void CAppForm::OnCustomdrawSliderSunIntensity(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pSun) m_pSun->SetSunIntensity(UIGetSunIntensity() * UIGetSunIntensityScale());
	
	*pResult = 0;
}

void CAppForm::OnUpdateEditSunIntensityScale() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView ::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pSun) m_pSun->SetSunIntensity(UIGetSunIntensity() * UIGetSunIntensityScale());
	
}

void CAppForm::OnUpdateEditBetaRayScale() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView ::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmBetaRayMultiplier, UIGetBetaRayMultiplier()*UIGetBetaRayScale());

	
}

void CAppForm::OnUpdateEditBetaMieScale() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView ::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmBetaMieMultiplier, UIGetBetaMieMultiplier()*UIGetBetaMieScale());
	
}

void CAppForm::OnCustomdrawSliderBetaMie(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmBetaMieMultiplier, UIGetBetaMieMultiplier()*UIGetBetaMieScale());
	
	*pResult = 0;
}

void CAppForm::OnCustomdrawSliderBetaRay(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmBetaRayMultiplier, UIGetBetaRayMultiplier()*UIGetBetaRayScale());
	
	*pResult = 0;
}

void CAppForm::OnCustomdrawSliderHgG(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmHGg, UIGetHGg());
	
	*pResult = 0;
}

void CAppForm::OnCustomdrawSliderMultExtinction(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmExtinctionMultiplier, UIGetExtinctionMultiplier());
	
	*pResult = 0;
}

void CAppForm::OnCustomdrawSliderMultInscattering(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 
	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmInscatteringMultiplier, UIGetInscatteringMultiplier());
	
	*pResult = 0;
}


void CAppForm::OnCheckMultExtinction() 
{
	// TODO: Add your control notification handler code here
	UpdateData(); 

	if (m_pAtmosphere) m_pAtmosphere->SetParam(eAtmExtinctionMultiplier, UIGetExtinctionMultiplier());
	
}


void CAppForm::OnCustomdrawSliderTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	float time = UIGetTime();
	float theta, phi;
	if (time < 0.5) 
		
	{
		// East direction
		phi = D3DXToRadian(270);
		// time 0.5 maps to zenith (theta = 0 degrees) and time 0 maps to east horizon (theta = 90 degrees)
		theta = 2*(0.5-time)*D3DXToRadian(90);
	}
	else
	{
		// West direction
		phi = D3DXToRadian(90);

		// time 0.5 maps to zenith (theta = 0 degrees) and time 1.0 maps to west horizon (theta = 90 degrees)
		theta = (time -0.5)*2*D3DXToRadian(90);
	}

	if (m_pSun) m_pSun->SetSunThetaPhi(theta, phi);
	
	*pResult = 0;
}

float CAppForm::ThetaPhi2Time(float theta, float phi)
{
	float time;
	if (phi >=  D3DXToRadian(180))
	{
		// in east horizon
		time = 0.5 - theta/(D3DXToRadian(90)*2);
	}
	else
	{
		time = theta/(D3DXToRadian(90)*2) + 0.5;
	}
	return time;
}


void CAppForm::UpdateUI()
{


	m_EditSunIntensityScale = 100;
	float inten1 = m_pSun->GetIntensity()/m_EditSunIntensityScale;
	m_SliderSunIntensity = 100 * inten1;

	m_SliderSunTheta = (int) (m_pSun->GetSunTheta() * 100.0f/D3DXToRadian(90));
	m_SliderSunPhi = (int) (m_pSun->GetSunPhi() * 100.0f/D3DXToRadian(360));
	if (m_SliderSunPhi > 100) m_SliderSunPhi -= 100;

	m_SliderTime = (int)(100.0f * ThetaPhi2Time(m_pSun->GetSunTheta(), m_pSun->GetSunPhi()));

	m_SliderBetaRay = (int)(100.0 * m_pAtmosphere->GetParam(eAtmBetaRayMultiplier));
	m_SliderBetaMie = (int)(100.0 * m_pAtmosphere->GetParam(eAtmBetaMieMultiplier));
	m_EditBetaRayScale = 1.0f;
	m_EditBetaMieScale = 1.0f;
	m_SliderHGg = (int)(100.0 * m_pAtmosphere->GetParam(eAtmHGg));
	
	m_CheckMultExtinction = (bool)(m_pAtmosphere->GetParam(eAtmExtinctionMultiplier));
	m_SliderMultInscattering = (int)(100.0 * m_pAtmosphere->GetParam(eAtmInscatteringMultiplier));


	UpdateData(false);
}


void CAppForm::DumpAnim()
{
	fprintf(m_fpDump,"1\n");
	m_pAnim->Dump2(m_fpDump);
}

void CAppForm::DumpMovie()
{
	m_pMovie->Dump2(m_fpRead);
}


void CAppForm::ReadMovie()
{
	m_pMovie->Read2(m_fpRead);
}

void CAppForm::OnHelp() 
{
	char strTitle[] = "About...";
	char strMsg[] = "\
           Real-Time Atmospheric\n\
               Light Scattering\n\
                           by\n\
\n\
       Naty Hoffman (Westwood)\n\
           (naty@westwood.com)\n\
                           &\n\
     Arcot Preetham (ATI Research)\n\
             (preetham@ati.com)\n\
\n\
Thanks to Kenny Mitchell's (Westwood)\n\
terrain engine and Solomon Srinivasan's \n\
(Paraform Inc.) help with the demo.\n\
";

  ::MessageBox( NULL, strMsg, strTitle, MB_OK );
}
