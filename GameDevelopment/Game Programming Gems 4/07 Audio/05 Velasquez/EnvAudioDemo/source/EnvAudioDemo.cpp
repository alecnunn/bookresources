//-----------------------------------------------------------------------------
// File: EnvAudioDemo.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <dinput.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "DIUtil.h"
#include "resource.h"
#include "EnvAudioDemo.h"

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// This GUID must be unique for every game, and the same for 
// every instance of this app.  // {4CD4C7CE-7230-40E0-9DC9-51ECEF7A2B83}
// The GUID allows DirectInput to remember input settings
GUID g_guidApp = { 0x4CD4C7CE, 0x7230, 0x40E0, { 0x9D, 0xC9, 0x51, 0xEC, 0xEF, 0x7A, 0x2B, 0x83 } };


// Input semantics used by this app
enum INPUT_SEMANTICS
{
    // Gameplay semantics
    // TODO: change as needed
    INPUT_ROTATE_AXIS_LR=1, INPUT_ROTATE_AXIS_UD,       
    INPUT_ROTATE_LEFT,      INPUT_ROTATE_RIGHT,    
    INPUT_ROTATE_UP,        INPUT_ROTATE_DOWN,
	INPUT_MOVE_FORWARD,		INPUT_MOVE_BACKWARD,
    INPUT_CONFIG_INPUT,     INPUT_CONFIG_DISPLAY,
    INPUT_PLAY_SOUND, 
	INPUT_MOVE_UP,			INPUT_MOVE_DOWN,
	INPUT_STRAFE_LEFT,		INPUT_STRAFE_RIGHT,
};

// Actions used by this app
DIACTION g_rgGameAction[] =
{
    // TODO: change as needed.  Be sure to delete user map files 
    // (C:\Program Files\Common Files\DirectX\DirectInput\User Maps\*.ini)
    // after changing this, otherwise settings won't reset and will be read 
    // from the out of date ini files 

    // Device input (joystick, etc.) that is pre-defined by DInput, according
    // to genre type. The genre for this app is space simulators.
    { INPUT_ROTATE_AXIS_LR,  DIAXIS_3DCONTROL_LATERAL,      0, TEXT("Rotate left/right"), },
    { INPUT_ROTATE_AXIS_UD,  DIAXIS_3DCONTROL_MOVE,         0, TEXT("Rotate up/down"), },
    { INPUT_PLAY_SOUND,      DIBUTTON_3DCONTROL_SPECIAL,    0, TEXT("Play sound"), },

	// Mouse input mappings
	//{ INPUT_ROTATE_AXIS_LR,	 DIMOUSE_XAXIS,					0, TEXT("Rotate left/right"), },

    // Keyboard input mappings
    { INPUT_ROTATE_LEFT,     DIKEYBOARD_LEFT,               0, TEXT("Rotate left"), },
    { INPUT_ROTATE_RIGHT,    DIKEYBOARD_RIGHT,              0, TEXT("Rotate right"), },
    { INPUT_ROTATE_UP,       DIKEYBOARD_UP,                 0, TEXT("Rotate up"), },
    { INPUT_ROTATE_DOWN,     DIKEYBOARD_DOWN,               0, TEXT("Rotate down"), },
	{ INPUT_MOVE_FORWARD,	 DIKEYBOARD_W,					0, TEXT("Move forward"), },
	{ INPUT_MOVE_BACKWARD,	 DIKEYBOARD_S,					0, TEXT("Move backward"), },
    { INPUT_PLAY_SOUND,      DIKEYBOARD_F5,                 0, TEXT("Play sound"), },
    { INPUT_CONFIG_DISPLAY,  DIKEYBOARD_F2,                 DIA_APPFIXED, TEXT("Configure Display"), },    
    { INPUT_CONFIG_INPUT,    DIKEYBOARD_F3,                 DIA_APPFIXED, TEXT("Configure Input"), },    
	{ INPUT_MOVE_UP,	     DIKEYBOARD_R,					0, TEXT("Move up"), },
	{ INPUT_MOVE_DOWN,	     DIKEYBOARD_F,					0, TEXT("Move down"), },
	{ INPUT_STRAFE_LEFT,     DIKEYBOARD_A,					0, TEXT("Strafe left"), },
	{ INPUT_STRAFE_RIGHT,    DIKEYBOARD_D,					0, TEXT("Strafe right"), },
};

#define NUMBER_OF_GAMEACTIONS    (sizeof(g_rgGameAction)/sizeof(DIACTION))




//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "EnvAudioDemo" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
	
	m_pMesh          = NULL;	
	m_pMeshMaterials = NULL;	
	m_pMeshTextures  = NULL;	
	m_dwNumMaterials = 0L;

    m_pInputDeviceManager       = NULL;
    m_pSoundManager             = NULL;

	for ( int i = 0; i < 8; i++ )
		m_pStreamSounds[i] = NULL;
	m_pZoomFXStreamSound = NULL;
	m_pLargeRoomSound = NULL;
	m_pSmallRoomSound = NULL;

    m_pDIConfigSurface          = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

	m_vWorldPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );		// Listener should spawn at (0, 25, 0) which puts them in large room

	m_bZoomFXEnabled = FALSE;

	// Read settings from registry
    ReadSettings();
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}



//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // TODO: perform one time initialization
	// Disable ZoomFX menu by default
	m_hMenu = GetMenu( m_hWnd );
	EnableMenuItem( m_hMenu, IDM_ENABLEZOOMFX, MF_GRAYED );

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    // Initialize DirectInput
    InitInput( m_hWnd );

    // Initialize audio
    InitAudio( m_hWnd );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadSettings()
// Desc: Read the app settings from the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::ReadSettings()
{
    HKEY hkey;
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Read the stored window width/height.  This is just an example,
        // of how to use DXUtil_Read*() functions.
        DXUtil_ReadIntRegKey( hkey, TEXT("Width"), &m_dwCreationWidth, m_dwCreationWidth );
        DXUtil_ReadIntRegKey( hkey, TEXT("Height"), &m_dwCreationHeight, m_dwCreationHeight );

        RegCloseKey( hkey );
    }
}




//-----------------------------------------------------------------------------
// Name: WriteSettings()
// Desc: Write the app settings to the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::WriteSettings()
{
    HKEY hkey;

    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Write the window width/height.  This is just an example,
        // of how to use DXUtil_Write*() functions.
        DXUtil_WriteIntRegKey( hkey, TEXT("Width"), m_rcWindowClient.right );
        DXUtil_WriteIntRegKey( hkey, TEXT("Height"), m_rcWindowClient.bottom );

        RegCloseKey( hkey );
    }
}





//-----------------------------------------------------------------------------
// Name: StaticInputAddDeviceCB()
// Desc: Static callback helper to call into CMyD3DApplication class
//-----------------------------------------------------------------------------
HRESULT CALLBACK CMyD3DApplication::StaticInputAddDeviceCB( 
                                         CInputDeviceManager::DeviceInfo* pDeviceInfo, 
                                         const DIDEVICEINSTANCE* pdidi, 
                                         LPVOID pParam )
{
    CMyD3DApplication* pApp = (CMyD3DApplication*) pParam;
    return pApp->InputAddDeviceCB( pDeviceInfo, pdidi );
}




//-----------------------------------------------------------------------------
// Name: InputAddDeviceCB()
// Desc: Called from CInputDeviceManager whenever a device is added. 
//       Set the dead zone, and creates a new InputDeviceState for each device
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, 
                                                   const DIDEVICEINSTANCE* pdidi )
{
    UNREFERENCED_PARAMETER( pdidi );
    
    // Setup the deadzone 
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = 500;
    pDeviceInfo->pdidDevice->SetProperty( DIPROP_DEADZONE, &dipdw.diph );

    // Create a new InputDeviceState for each device so the 
    // app can record its state 
    InputDeviceState* pNewInputDeviceState = new InputDeviceState;
    ZeroMemory( pNewInputDeviceState, sizeof(InputDeviceState) );
    pDeviceInfo->pParam = (LPVOID) pNewInputDeviceState;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitInput()
// Desc: Initialize DirectInput objects
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitInput( HWND hWnd )
{
    HRESULT hr;

    // Setup action format for the actual gameplay
    ZeroMemory( &m_diafGame, sizeof(DIACTIONFORMAT) );
    m_diafGame.dwSize          = sizeof(DIACTIONFORMAT);
    m_diafGame.dwActionSize    = sizeof(DIACTION);
    m_diafGame.dwDataSize      = NUMBER_OF_GAMEACTIONS * sizeof(DWORD);
    m_diafGame.guidActionMap   = g_guidApp;

    // TODO: change the genre as needed
    m_diafGame.dwGenre         = DIVIRTUAL_CAD_3DCONTROL; 

    m_diafGame.dwNumActions    = NUMBER_OF_GAMEACTIONS;
    m_diafGame.rgoAction       = g_rgGameAction;
    m_diafGame.lAxisMin        = -100;
    m_diafGame.lAxisMax        = 100;
    m_diafGame.dwBufferSize    = 16;
    _tcscpy( m_diafGame.tszActionMap, _T("EnvAudioDemo Game") );

    // Create a new input device manager
    m_pInputDeviceManager = new CInputDeviceManager();

    if( FAILED( hr = m_pInputDeviceManager->Create( hWnd, NULL, m_diafGame, 
                                                    StaticInputAddDeviceCB, this ) ) )
        return DXTRACE_ERR( "m_pInputDeviceManager->Create", hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitAudio()
// Desc: Initialize DirectX audio objects
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitAudio( HWND hWnd )
{
    HRESULT hr;

    // Create a static IDirectSound in the CSound class.  
    // Set coop level to DSSCL_PRIORITY, and set primary buffer 
    // format to stereo, 22kHz and 16-bit output.
    m_pSoundManager = new CSoundManager();

    if( FAILED( hr = m_pSoundManager->Initialize( hWnd, DSSCL_PRIORITY ) ) )
        return DXTRACE_ERR( TEXT("m_pSoundManager->Initialize"), hr );

    if( FAILED( hr = m_pSoundManager->SetPrimaryBufferFormat( 2, 44100, 16 ) ) )
        return DXTRACE_ERR( TEXT("m_pSoundManager->SetPrimaryBufferFormat"), hr );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: OnOpenLevel()
// Desc: Called when a user selects Open Level from File menu
//-----------------------------------------------------------------------------
void CMyD3DApplication::OnOpenLevel( 
	void )
{
	TCHAR strFilename[512]   = _T("");

    // Display the OpenFileName dialog. Then, try to load the specified file
    OPENFILENAME ofn = { sizeof(OPENFILENAME), NULL, NULL,
                        _T(".X Files (.x)\0*.x\0\0"), 
                        NULL, 0, 1, NULL, 512, strFilename, 512, 
                        NULL, _T("Open Mesh File"), 
                        OFN_FILEMUSTEXIST, 0, 1, NULL, 0, NULL, NULL };

	// Display the Open dialog box. 

	if ( GetOpenFileName(&ofn) == TRUE ) 
	{
		// Destroy and re-create everything
		//InvalidateDeviceObjects();
		//DeleteDeviceObjects();
	}

    CMyD3DApplication::InitDeviceObjects();
    RestoreDeviceObjects();

	// Load .X file and create mesh object
	if ( LoadModel( strFilename ) == S_OK )
	{
		HRESULT hr;

		if( SUCCEEDED( hr = m_pSoundManager->InitializeEffects() ) )
		{
			if ( (m_pSoundManager->GetHardwareSupport() & SND_HDWR_ZOOMFX_SOURCE_SUPPORT) )
			{
				m_hMenu = GetMenu( m_hWnd );
				EnableMenuItem( m_hMenu, IDM_ENABLEZOOMFX, MF_ENABLED );	
			}
			
			////////////////////////////////////////////////
			// Now that level data is loaded, create sounds
			////////////////////////////////////////////////
			if ( SUCCEEDED(m_pSoundManager->Create( &m_pLargeRoomSound, TEXT("siren.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pLargeRoomSound->SetLocation( D3DXVECTOR3(-250.183f, 17.5f, 250.59f ) );
				// Set min/max distance and build geometry representation
				m_pLargeRoomSound->SetMinMaxDistance( 35.0f, 5000.0f );
				m_pLargeRoomSound->BuildGeometry( m_pd3dDevice );
				m_pLargeRoomSound->Play( 0, DSBPLAY_LOOPING, -100);
			}

			
			if ( SUCCEEDED(m_pSoundManager->Create( &m_pSmallRoomSound, TEXT("radio.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pSmallRoomSound->SetLocation( D3DXVECTOR3( 299.245f, 15.0f, 185.231f) );
				// Set min/max distance and build geometry representation
				m_pSmallRoomSound->SetMinMaxDistance( 30.0f, 3000.0f );
				m_pSmallRoomSound->BuildGeometry( m_pd3dDevice );
				m_pSmallRoomSound->Play( 0, DSBPLAY_LOOPING);
			}

			// This is the only sound we'll try and create with ZoomFX support
			if ( SUCCEEDED(m_pSoundManager->Create( &m_pZoomFXStreamSound, TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME, GUID_NULL, 1, TRUE )) )
			{
				ZoomFXProperties props;
				props.Reset();

				props.box.vMin.x = 795.0f;
				props.box.vMin.y = 0.0f;
				props.box.vMin.z = 90.0f;
				props.box.vMax.x = -795.0f;
				props.box.vMax.y = 30.0f;
				props.box.vMax.z = -90.0f;
							
				m_pZoomFXStreamSound->SetLocation( D3DXVECTOR3( -15.0f, 15.0f, 800.0f ) );
				// Set min/max distance and build geometry representation
				m_pZoomFXStreamSound->SetZoomFXProperties( props );
				LPDIRECTSOUND3DLISTENER pListener;
				m_pSoundManager->Get3DListenerInterface( &pListener );
				if ( pListener )
					pListener->CommitDeferredSettings();
				m_pZoomFXStreamSound->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pZoomFXStreamSound->BuildGeometry( m_pd3dDevice );
				props.Reset();
				m_pZoomFXStreamSound->SetZoomFXProperties( props );
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[0], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[0]->SetLocation( D3DXVECTOR3( 670.88f, 15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[0]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[0]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[0]->Play( 0, DSBPLAY_LOOPING);
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[1], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[1]->SetLocation( D3DXVECTOR3( 457.472f, 15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[1]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[1]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[1]->Play( 0, DSBPLAY_LOOPING);
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[2], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[2]->SetLocation( D3DXVECTOR3( 257.1f,  15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[2]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[2]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[2]->Play( 0, DSBPLAY_LOOPING);
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[3], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[3]->SetLocation( D3DXVECTOR3( 58.051f, 15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[3]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[3]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[3]->Play( 0, DSBPLAY_LOOPING);
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[4], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[4]->SetLocation( D3DXVECTOR3( -136.743f, 15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[4]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[4]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[4]->Play( 0, DSBPLAY_LOOPING);
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[5], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[5]->SetLocation( D3DXVECTOR3( -327.279f,  15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[5]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[5]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[5]->Play( 0, DSBPLAY_LOOPING);
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[6], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[6]->SetLocation( D3DXVECTOR3( -516.485f, 15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[6]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[6]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[6]->Play( 0, DSBPLAY_LOOPING);
			}

			if ( SUCCEEDED(m_pSoundManager->Create( &m_pStreamSounds[7], TEXT("stream.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME )) )
			{
				m_pStreamSounds[7]->SetLocation( D3DXVECTOR3( -707.29f,  15.0f, 795.174f ) );
				// Set min/max distance and build geometry representation
				m_pStreamSounds[7]->SetMinMaxDistance( 30.0f, 1000.0f );
				m_pStreamSounds[7]->BuildGeometry( m_pd3dDevice );
				m_pStreamSounds[7]->Play( 0, DSBPLAY_LOOPING);
			}

			// Initialize the listener's environment
			if ( m_pSoundManager->LoadEAXLevelData( strFilename ) == S_OK )
				m_pSoundManager->InitializeListenerAttributes( m_vWorldPos, m_vForward, m_vUp );
		}
	}

	return;
}

//-----------------------------------------------------------------------------
// Name: LoadModel()
// Desc: Called by Open Level to load the mesh and build the 
//		 material and texture arrays
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::LoadModel( 
	char* szFileName )
{
	HRESULT hr;
	LPD3DXBUFFER pD3DXMtrlBuffer;
	TCHAR        strMesh[512];

	// Get a path to the media file
    if( FAILED( hr = DXUtil_FindMediaFileCb( strMesh, sizeof(strMesh), szFileName ) ) )
	{
		MessageBox(NULL, "Could not find model", "Missing file", MB_OK);
		return DXTRACE_ERR( "DXUtil_FindMediaFileCb", hr );
	}

    // Load the mesh from the specified file
	if( FAILED( hr = D3DXLoadMeshFromX( strMesh, D3DXMESH_SYSTEMMEM, 
                                   m_pd3dDevice, NULL, 
                                   &pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, 
                                   &m_pMesh ) ) )
    {
       MessageBox(NULL, "Could not load model", "Missing or corrupt file", MB_OK);
	   return DXTRACE_ERR( "D3DXLoadMeshFromX", hr );
    }

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
    m_pMeshTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

    for( DWORD i=0; i<m_dwNumMaterials; i++ )
    {
        // Copy the material
        m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

        m_pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL && 
            lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {
            // 3dsmax usually stores texture names with the full path name so change this to 
			// fit our needs ./textures
			char szNewTextureFilename[260];
			char* pdest = NULL;
			int result;

			ZeroMemory(szNewTextureFilename, sizeof(char) * 260);

			// Tack on our ./textures path
			sprintf( szNewTextureFilename, ".\\textures\\" );

			pdest = strrchr( d3dxMaterials[i].pTextureFilename, '\\' );
			result = (int)(pdest - d3dxMaterials[i].pTextureFilename + 1);
			
			if ( pdest != NULL )
			{
				// Found a backslash so let's delete it and everything before it
				strcat( szNewTextureFilename, d3dxMaterials[i].pTextureFilename + result );
			}

			// Create the texture
            if( FAILED( hr = D3DXCreateTextureFromFile( m_pd3dDevice, 
												szNewTextureFilename,
                                                /*d3dxMaterials[i].pTextureFilename,*/ 
                                                &m_pMeshTextures[i] ) ) )
            {
                MessageBox(NULL, "Could not find texture map", "Missing texture", MB_OK);
				DXTRACE_ERR( "D3DXCreateTextureFromFile", hr );
            }
        }
    }

    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateOffScreenPlainSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // TODO: create device objects

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	//DrawSphere();
	//InitVB();
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // TODO: setup render states
    HRESULT hr;

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the 
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	m_vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &m_vWorldPos/*&vEyePt*/, &vLookatPt, &m_vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 10.0f, 100000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up lighting states
    //D3DLIGHT9 light;
    //D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
	//m_pd3dDevice->SetLight( 0, &light );
	//m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	//m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x003f3f3f );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    if( !m_bWindowed )
    {
        // Create a surface for configuring DInput devices
        if( FAILED( hr = m_pd3dDevice->CreateOffscreenPlainSurface( 640, 480, 
                                        m_d3dsdBackBuffer.Format, D3DPOOL_DEFAULT, 
										&m_pDIConfigSurface, NULL ) ) ) 
            return DXTRACE_ERR( "CreateOffscreenPlainSurface", hr );
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
#define MOVEMENT_DELTA_SPEED	125
#define ANGLE_DELTA_SPEED		100

HRESULT CMyD3DApplication::SetupMatrices( void )
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// World Transformation
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// For vertices that are defined in world coordinates, the world matrix should be set to identity.
	// For moving objects, we'll want to base the world matrix off of orientation and position of the 
	// moving objects.

    D3DXMATRIXA16 matWorld;

	D3DXMatrixIdentity( &matWorld );
	// the models are coming in rotated, so lets help them
	D3DXMatrixRotationYawPitchRoll( &matWorld, 0.0f, D3DXToRadian( -90.0f ), 0.0f );

	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// View Transformation
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	D3DXMATRIXA16 matView;

	if( m_UserInput.fAxisRotateLR )
        m_fWorldRotY += m_fElapsedTime * m_UserInput.fAxisRotateLR * ANGLE_DELTA_SPEED;

    if( m_UserInput.fAxisRotateUD )
        m_fWorldRotX += m_fElapsedTime * m_UserInput.fAxisRotateUD * ANGLE_DELTA_SPEED;

	if( m_fWorldRotY > 360.0f )	{ m_fWorldRotY -= 360.0f; }
	if( m_fWorldRotY < 0.0f )	{ m_fWorldRotY += 360.0f; }

	if( m_fWorldRotX > 360.0f )	{ m_fWorldRotX -= 360.0f; }
	if( m_fWorldRotX < 0.0f )	{ m_fWorldRotX += 360.0f; }
	
	D3DXMatrixRotationYawPitchRoll( &matView, D3DXToRadian( -m_fWorldRotY ), D3DXToRadian( -m_fWorldRotX ), 0.0f );

	m_vForward = D3DXVECTOR3( matView._31, matView._32, matView._33 );
	m_vUp = D3DXVECTOR3( matView._21, matView._22, matView._23 );
	D3DXVECTOR3 vRight( matView._11, matView._12, matView._13 );

	if( m_UserInput.fMoveFB )
	{
		m_vWorldPos += m_vForward * m_fElapsedTime * m_UserInput.fMoveFB * MOVEMENT_DELTA_SPEED;
	}

	if( m_UserInput.fMoveUD )
	{
		m_vWorldPos += m_vUp * m_fElapsedTime * m_UserInput.fMoveUD * MOVEMENT_DELTA_SPEED;
	}

	if ( m_UserInput.fStrafeLR )
	{
		m_vWorldPos += vRight * m_fElapsedTime * m_UserInput.fStrafeLR * MOVEMENT_DELTA_SPEED;
	}

	D3DXVECTOR3 vAt = m_vWorldPos + m_vForward * 32.0f;

	D3DXMatrixLookAtLH( &matView, &m_vWorldPos, &vAt, &m_vUp );

    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Projection
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 4096.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: StaticConfigureInputDevicesCB()
// Desc: Static callback helper to call into CMyD3DApplication class
//-----------------------------------------------------------------------------
BOOL CALLBACK CMyD3DApplication::StaticConfigureInputDevicesCB( 
                                            IUnknown* pUnknown, VOID* pUserData )
{
    CMyD3DApplication* pApp = (CMyD3DApplication*) pUserData;
    return pApp->ConfigureInputDevicesCB( pUnknown );
}




//-----------------------------------------------------------------------------
// Name: ConfigureInputDevicesCB()
// Desc: Callback function for configuring input devices. This function is
//       called in fullscreen modes, so that the input device configuration
//       window can update the screen.
//-----------------------------------------------------------------------------
BOOL CMyD3DApplication::ConfigureInputDevicesCB( IUnknown* pUnknown )
{
    // Get access to the surface
    LPDIRECT3DSURFACE9 pConfigSurface = NULL;
    if( FAILED( pUnknown->QueryInterface( IID_IDirect3DSurface9,
                                          (VOID**)&pConfigSurface ) ) )
        return TRUE;

    // Render the scene, with the config surface blitted on top
    Render();

    RECT  rcSrc;
    SetRect( &rcSrc, 0, 0, 640, 480 );

    POINT ptDst;
    ptDst.x = (m_d3dsdBackBuffer.Width-640)/2;
    ptDst.y = (m_d3dsdBackBuffer.Height-480)/2;

    LPDIRECT3DSURFACE9 pBackBuffer;
    m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    m_pd3dDevice->UpdateSurface( pConfigSurface, &rcSrc, pBackBuffer, &ptDst );
    pBackBuffer->Release();

    // Present the backbuffer contents to the front buffer
    m_pd3dDevice->Present( 0, 0, 0, 0 );

    // Release the surface
    pConfigSurface->Release();

    return TRUE;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // Update user input state
    UpdateInput( &m_UserInput );

    // Respond to input
    if( m_UserInput.bDoConfigureInput && m_bWindowed )  // full-screen configure disabled for now
    {
        // One-shot per keypress
        m_UserInput.bDoConfigureInput = FALSE;

        Pause( true );

        // Get access to the list of semantically-mapped input devices
        // to delete all InputDeviceState structs before calling ConfigureDevices()
        CInputDeviceManager::DeviceInfo* pDeviceInfos;
        DWORD dwNumDevices;
        m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

        for( DWORD i=0; i<dwNumDevices; i++ )
        {
            InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;
            SAFE_DELETE( pInputDeviceState );
            pDeviceInfos[i].pParam = NULL;
        }

        // Configure the devices (with edit capability)
        if( m_bWindowed )
            m_pInputDeviceManager->ConfigureDevices( m_hWnd, NULL, NULL, DICD_EDIT, NULL );
        else
            m_pInputDeviceManager->ConfigureDevices( m_hWnd,
                                                     m_pDIConfigSurface,
                                                     (VOID*)StaticConfigureInputDevicesCB,
                                                     DICD_EDIT, (LPVOID) this );

        Pause( false );
    }

    if( m_UserInput.bDoConfigureDisplay )
    {
        // One-shot per keypress
        m_UserInput.bDoConfigureDisplay = FALSE;

        Pause(true);

        // Configure the display device
        UserSelectNewDevice();

        Pause(false);
    }

    // Update the world state according to user input
	SetupMatrices();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    if( NULL == m_pInputDeviceManager )
        return;

    // Get access to the list of semantically-mapped input devices
    CInputDeviceManager::DeviceInfo* pDeviceInfos;
    DWORD dwNumDevices;
    m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

    // Loop through all devices and check game input
    for( DWORD i=0; i<dwNumDevices; i++ )
    {
        DIDEVICEOBJECTDATA rgdod[10];
        DWORD   dwItems = 10;
        HRESULT hr;
        LPDIRECTINPUTDEVICE8 pdidDevice = pDeviceInfos[i].pdidDevice;
        InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;

        hr = pdidDevice->Acquire();
        hr = pdidDevice->Poll();
        hr = pdidDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA),
                                        rgdod, &dwItems, 0 );
        if( FAILED(hr) )
            continue;

        // Get the sematics codes for the game menu
        for( DWORD j=0; j<dwItems; j++ )
        {
            BOOL  bButtonState = (rgdod[j].dwData==0x80) ? TRUE : FALSE;
            FLOAT fButtonState = (rgdod[j].dwData==0x80) ? 1.0f : 0.0f;
            FLOAT fAxisState   = (FLOAT)((int)rgdod[j].dwData)/100.0f;
            UNREFERENCED_PARAMETER( fButtonState );

            switch( rgdod[j].uAppData )
            {
                // TODO: Handle semantics for the game 

                // Handle relative axis data
                case INPUT_ROTATE_AXIS_LR: 
                    pInputDeviceState->fAxisRotateLR = -fAxisState;
                    break;
                case INPUT_ROTATE_AXIS_UD:
                    pInputDeviceState->fAxisRotateUD = -fAxisState;
                    break;

                // Handle buttons separately so the button state data
                // doesn't overwrite the axis state data, and handle
                // each button separately so they don't overwrite each other
                case INPUT_ROTATE_LEFT:  pInputDeviceState->bButtonRotateLeft  = bButtonState; break;
                case INPUT_ROTATE_RIGHT: pInputDeviceState->bButtonRotateRight = bButtonState; break;
                case INPUT_ROTATE_UP:    pInputDeviceState->bButtonRotateUp    = bButtonState; break;
                case INPUT_ROTATE_DOWN:  pInputDeviceState->bButtonRotateDown  = bButtonState; break;
				case INPUT_MOVE_FORWARD: pInputDeviceState->bButtonMoveForward = bButtonState; break;
				case INPUT_MOVE_BACKWARD: pInputDeviceState->bButtonMoveBackward = bButtonState; break;
                case INPUT_PLAY_SOUND:   pInputDeviceState->bButtonPlaySoundButtonDown = bButtonState; break;

                // Handle one-shot buttons
                case INPUT_CONFIG_INPUT:   if( bButtonState ) pUserInput->bDoConfigureInput = TRUE; break;
                case INPUT_CONFIG_DISPLAY: if( bButtonState ) pUserInput->bDoConfigureDisplay = TRUE; break;

				case INPUT_MOVE_UP: pInputDeviceState->bButtonMoveUp = bButtonState; break;
				case INPUT_MOVE_DOWN: pInputDeviceState->bButtonMoveDown = bButtonState; break;
				case INPUT_STRAFE_LEFT: pInputDeviceState->bButtonStrafeLeft = bButtonState; break;
				case INPUT_STRAFE_RIGHT: pInputDeviceState->bButtonStrafeRight = bButtonState; break;
            }
        }
    }

    // TODO: change process code as needed

    // Process user input and store result into pUserInput struct
    pUserInput->fAxisRotateLR = 0.0f;
    pUserInput->fAxisRotateUD = 0.0f;
    pUserInput->bPlaySoundButtonDown = FALSE;
	pUserInput->fMoveFB = 0.0f;
	pUserInput->fMoveUD = 0.0f;
	pUserInput->fStrafeLR = 0.0f;

    // Concatinate the data from all the DirectInput devices
    for( i=0; i<dwNumDevices; i++ )
    {
        InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;

        // Use the axis data that is furthest from zero
        if( fabs(pInputDeviceState->fAxisRotateLR) > fabs(pUserInput->fAxisRotateLR) )
            pUserInput->fAxisRotateLR = pInputDeviceState->fAxisRotateLR;

        if( fabs(pInputDeviceState->fAxisRotateUD) > fabs(pUserInput->fAxisRotateUD) )
            pUserInput->fAxisRotateUD = pInputDeviceState->fAxisRotateUD;

        // Process the button data 
        if( pInputDeviceState->bButtonRotateLeft )
            pUserInput->fAxisRotateLR = 1.0f;
        else if( pInputDeviceState->bButtonRotateRight )
            pUserInput->fAxisRotateLR = -1.0f;

        if( pInputDeviceState->bButtonRotateUp )
            pUserInput->fAxisRotateUD = 1.0f;
        else if( pInputDeviceState->bButtonRotateDown )
            pUserInput->fAxisRotateUD = -1.0f;

		if( pInputDeviceState->bButtonMoveForward )
            pUserInput->fMoveFB = 1.0f;
        else if( pInputDeviceState->bButtonMoveBackward )
            pUserInput->fMoveFB = -1.0f;

		if( pInputDeviceState->bButtonMoveUp )
            pUserInput->fMoveUD = 1.0f;
		else if( pInputDeviceState->bButtonMoveDown )
			pUserInput->fMoveUD = -1.0f;

		if( pInputDeviceState->bButtonStrafeLeft )
            pUserInput->fStrafeLR = -1.0f;
		else if( pInputDeviceState->bButtonStrafeRight )
			pUserInput->fStrafeLR = 1.0f;

        if( pInputDeviceState->bButtonPlaySoundButtonDown )
            pUserInput->bPlaySoundButtonDown = TRUE;
    } 
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// Render world
		// Meshes are divided into subsets, one for each material. Render them in
        // a loop
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            // Set the material and texture for this subset
            m_pd3dDevice->SetMaterial( &m_pMeshMaterials[i] );
			m_pd3dDevice->SetTexture( 0, m_pMeshTextures[i] );
        
            // Draw the mesh subset
            m_pMesh->DrawSubset( i );
        }
        
		D3DMATERIAL9 mat;
		ZeroMemory( &mat, sizeof(D3DMATERIAL9) );
		mat.Diffuse.a = 1.0;
		mat.Diffuse.r = 1.0;
		mat.Diffuse.g = 1.0;
		mat.Diffuse.b = 1.0;
		m_pd3dDevice->SetMaterial(&mat);

		m_pd3dDevice->SetTexture( 0, NULL );

		if ( m_pLargeRoomSound )
			m_pLargeRoomSound->Draw( m_pd3dDevice );

		if ( m_pSmallRoomSound )
			m_pSmallRoomSound->Draw( m_pd3dDevice );

		if ( m_bZoomFXEnabled )
		{
			if ( m_pZoomFXStreamSound )
				m_pZoomFXStreamSound->Draw( m_pd3dDevice );
		}
		else
		{
			for ( int i = 0; i < 8; i++ )
			{
				if ( m_pStreamSounds[i] )
					m_pStreamSounds[i]->Draw( m_pd3dDevice );
			}
		}

        // Render stats and help text  
        RenderText();

		// Update the listener object
		m_pSoundManager->UpdateListener( m_vWorldPos, m_vForward, m_vUp );

		// And update all the EAX sounds
		if ( m_pLargeRoomSound )
			m_pSoundManager->UpdateSource( m_pLargeRoomSound );
		if ( m_pSmallRoomSound )
			m_pSoundManager->UpdateSource( m_pSmallRoomSound );
		if ( m_bZoomFXEnabled )
		{
			if ( m_pZoomFXStreamSound )
				m_pSoundManager->UpdateSource( m_pZoomFXStreamSound );
		}
		else
		{
			for ( int j = 0; j < 8; j++ )
			{
				if ( m_pStreamSounds[j] )
					m_pSoundManager->UpdateSource( m_pStreamSounds[j] );
			}
		}

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	sprintf( szMsg, TEXT("roation  = %f %f %f"), m_fWorldRotX, m_fWorldRotY, 0.0f );
    m_pFont->DrawText( 2, 60, fontColor, szMsg );
    sprintf( szMsg, TEXT("forward  = %f %f %f"), m_vForward.x, m_vForward.y, m_vForward.z );
    m_pFont->DrawText( 2, 80, fontColor, szMsg );
    sprintf( szMsg, TEXT("position = %f %f %f"), m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z );
    m_pFont->DrawText( 2, 100, fontColor, szMsg );


	// Output statistics 
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    
    // Get listener environment string to display
    if ( m_pSoundManager )
    {
        TCHAR szPreset[MAX_PATH] = TEXT("");
		lstrcpy( szMsg, TEXT("Environment Preset: ") );
		m_pSoundManager->GetEnvironmentPresetString( (TCHAR*)&szPreset );
		if ( lstrlen( szPreset ) )
			lstrcat( szMsg, szPreset );
		else
			lstrcat( szMsg, TEXT("None") );
		fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    }
 
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::Pause( bool bPause )
{
    if ( m_pInputDeviceManager != NULL )
	{
		
		// Get access to the list of semantically-mapped input devices
		// to zero the state of all InputDeviceState structs.  This is needed
		// because when using DISCL_FOREGROUND, the action mapper will not 
		// record actions when the focus switches, for example if a dialog appears.
		// This causes a problem when a button held down when loosing focus, and let
		// go when the focus is lost.  The app will not record that the button 
		// has been let go, so the state will be incorrect when focus returns.  
		// To fix this either use DISCL_BACKGROUND or zero the state when 
		// loosing focus.
		CInputDeviceManager::DeviceInfo* pDeviceInfos;
		DWORD dwNumDevices;
		m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

		for( DWORD i=0; i<dwNumDevices; i++ )
		{
			InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;
			ZeroMemory( pInputDeviceState, sizeof(InputDeviceState) );
		}

		CD3DApplication::Pause( bPause );
	}
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

        case WM_COMMAND:
        {
            switch( LOWORD(wParam) )
            {
                case IDM_CONFIGINPUT:
                    m_UserInput.bDoConfigureInput = TRUE;
                    break;

                case IDM_CHANGEDEVICE:
                    m_UserInput.bDoConfigureDisplay = TRUE;
                    return 0; // Don't hand off to parent

				case IDM_ENABLEZOOMFX:
					OnEnableZoomFX();
					return 0;

				// User wants to open a .x file
				case ID_FILE_OPENLEVEL:
					OnOpenLevel();
					return 0;
			}
            break;
        }


    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();
    SAFE_RELEASE( m_pDIConfigSurface );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // TODO: Cleanup any objects created in InitDeviceObjects()
    m_pFont->DeleteDeviceObjects();
	
    if( m_pMeshMaterials != NULL ) 
        delete[] m_pMeshMaterials;

    if( m_pMeshTextures )
    {
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
        {
            if( m_pMeshTextures[i] )
                m_pMeshTextures[i]->Release();
        }
        delete[] m_pMeshTextures;
    }

    if( m_pMesh != NULL )
        m_pMesh->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    // Cleanup DirectInput
    CleanupDirectInput();

    // Cleanup DirectX audio objects
    for ( int i = 0; i < 8; i++ )
	{
        if ( m_pStreamSounds[i] )
		{
			delete m_pStreamSounds[i];
			m_pStreamSounds[i] = NULL;
		}
	}
	
	SAFE_DELETE( m_pZoomFXStreamSound );
	SAFE_DELETE( m_pLargeRoomSound );
	SAFE_DELETE( m_pSmallRoomSound );
    SAFE_DELETE( m_pSoundManager );

    // Write the settings to the registry
    WriteSettings();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CleanupDirectInput()
// Desc: Cleanup DirectInput 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::CleanupDirectInput()
{
    if( NULL == m_pInputDeviceManager )
        return;

    // Get access to the list of semantically-mapped input devices
    // to delete all InputDeviceState structs
    CInputDeviceManager::DeviceInfo* pDeviceInfos;
    DWORD dwNumDevices;
    m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

    for( DWORD i=0; i<dwNumDevices; i++ )
    {
        InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;
        SAFE_DELETE( pInputDeviceState );
        pDeviceInfos[i].pParam = NULL;
    }

    // Cleanup DirectX input objects
    SAFE_DELETE( m_pInputDeviceManager );

}

//-----------------------------------------------------------------------------
// Name: OnEnableZoomFX
// Desc: Either enables or disables ZoomFX based on menu selection.
//-----------------------------------------------------------------------------
void CMyD3DApplication::OnEnableZoomFX( void ) 
{
	if ( (m_pSoundManager->GetHardwareSupport() & SND_HDWR_ZOOMFX_SOURCE_SUPPORT) )
	{
		m_bZoomFXEnabled = !m_bZoomFXEnabled;
		if ( m_bZoomFXEnabled )	
		{
			// Enabling, so send properties to hardware 
			ZoomFXProperties props;
			props.Reset();
			props.box.vMin.x = 795.0f;
			props.box.vMin.y = 0.0f;
			props.box.vMin.z = 90.0f;
			props.box.vMax.x = -795.0f;
			props.box.vMax.y = 30.0f;
			props.box.vMax.z = -90.0f;
			m_pZoomFXStreamSound->SetZoomFXProperties( props );

			for ( int i = 0; i < 8; i++ )
			{
				if( m_pStreamSounds[i] )
					m_pStreamSounds[i]->Stop();
			}

			if ( m_pZoomFXStreamSound )
				m_pZoomFXStreamSound->Play( 0, DSBPLAY_LOOPING);

			// Check the menu item
			m_hMenu = GetMenu( m_hWnd );
			CheckMenuItem( m_hMenu, IDM_ENABLEZOOMFX, MF_CHECKED );
		}
		else	
		{
			// Disable ZoomFX by sending a NULL size bounding box
			ZoomFXProperties props;
			props.Reset();
			m_pZoomFXStreamSound->SetZoomFXProperties( props );

			if ( m_pZoomFXStreamSound )
				m_pZoomFXStreamSound->Stop();
						
			for ( int i = 0; i < 8; i++ )
			{
				if( m_pStreamSounds[i] )
					m_pStreamSounds[i]->Play( 0, DSBPLAY_LOOPING);
			}


			// Uncheck the menu item
			m_hMenu = GetMenu( m_hWnd );
			CheckMenuItem( m_hMenu, IDM_ENABLEZOOMFX, MF_UNCHECKED );
		}
	}
}