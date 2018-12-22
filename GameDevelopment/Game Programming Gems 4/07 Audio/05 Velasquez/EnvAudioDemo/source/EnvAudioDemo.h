//-----------------------------------------------------------------------------
// File: EnvAudioDemo.h
//
// Desc: Header file EnvAudioDemo sample app
//-----------------------------------------------------------------------------
#pragma once

#include "CSoundManager.h"


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\EnvAudioDemo")

// DirectInput action mapper reports events only when buttons/axis change
// so we need to remember the present state of relevant axis/buttons for 
// each DirectInput device.  The CInputDeviceManager will store a 
// pointer for each device that points to this struct
struct InputDeviceState
{
    // TODO: change as needed
    FLOAT fAxisRotateLR;
    BOOL  bButtonRotateLeft;
    BOOL  bButtonRotateRight;

    FLOAT fAxisRotateUD;
    BOOL  bButtonRotateUp;
    BOOL  bButtonRotateDown;

	FLOAT fMoveFB;
	BOOL  bButtonMoveForward;
	BOOL  bButtonMoveBackward;

	FLOAT fMoveUD;
	BOOL  bButtonMoveUp;
	BOOL  bButtonMoveDown;

	FLOAT fStrafeLR;
	BOOL  bButtonStrafeLeft;
	BOOL  bButtonStrafeRight;

	FLOAT fAxisMoveLR;

    BOOL  bButtonPlaySoundButtonDown;
};


// Struct to store the current input state
struct UserInput
{
    // TODO: change as needed
    FLOAT fAxisRotateUD;
    FLOAT fAxisRotateLR;
	FLOAT fMoveFB;
	FLOAT fMoveUD;
	FLOAT fStrafeLR;
	FLOAT fAxisMoveLR;
    BOOL bPlaySoundButtonDown;
    BOOL bDoConfigureInput;
    BOOL bDoConfigureDisplay;
};




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;					// TRUE, if the app is loading
    CD3DFont*               m_pFont;						// Font for drawing text
    ID3DXMesh*              m_pD3DXMesh;					// D3DX mesh to store teapot

	LPD3DXMESH              m_pMesh;						// Our mesh object in sysmem
	D3DMATERIAL9*           m_pMeshMaterials;				// Materials for our mesh
	LPDIRECT3DTEXTURE9*     m_pMeshTextures;				// Textures for our mesh
	DWORD                   m_dwNumMaterials;				// Number of mesh materials

    CInputDeviceManager*    m_pInputDeviceManager;			// DirectInput device manager
    DIACTIONFORMAT          m_diafGame;						// Action format for game play
    LPDIRECT3DSURFACE9      m_pDIConfigSurface;				// Surface for config'ing DInput devices
    UserInput               m_UserInput;					// Struct for storing user input 

    FLOAT                   m_fSoundPlayRepeatCountdown;	// Sound repeat timer
    CSoundManager*          m_pSoundManager;				// DirectSound manager class
	CSound*					m_pStreamSounds[8];				// Non-ZoomFX stream sounds
	CSound*					m_pZoomFXStreamSound;			// ZoomFX stream sound
	CSound*					m_pLargeRoomSound;				// Large room sound
	CSound*					m_pSmallRoomSound;				// Small room sound
    FLOAT                   m_fWorldRotX;					// World rotation state X-axis
    FLOAT                   m_fWorldRotY;					// World rotation state Y-axis
	D3DXVECTOR3				m_vWorldPos;
	D3DXVECTOR3				m_vForward;
	D3DXVECTOR3				m_vUp;
	D3DXMATRIX				m_matForward;
	BOOL					m_bZoomFXEnabled;


protected:
    virtual HRESULT OneTimeSceneInit();
	HRESULT SetupMatrices(void);
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );
    VOID    Pause( bool bPause );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();
    HRESULT InitAudio( HWND hWnd );

	void	OnOpenLevel( void );					// Handle user wanting to load a .x file
	HRESULT LoadModel( TCHAR* szFileName );		// Load .X file and create mesh object
	void	OnEnableZoomFX( void ); 

    VOID    ReadSettings();
    VOID    WriteSettings();

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, const DIDEVICEINSTANCE* pdidi );
    static HRESULT CALLBACK StaticInputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, const DIDEVICEINSTANCE* pdidi, LPVOID pParam );   
    BOOL    ConfigureInputDevicesCB( IUnknown* pUnknown );
    static BOOL CALLBACK StaticConfigureInputDevicesCB( IUnknown* pUnknown, VOID* pUserData );
};

