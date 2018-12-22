#ifndef CSOUNDMANAGER_H
#define CSOUNDMANAGER_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <d3dx9math.h>

// EAX Manager object
#include "eaxman.h"

// Sound effect objects
#include "CListenerEffectEAX.h"

#include "CSound.h"

#define DSUtil_StopSound(s)         { if(s) s->Stop(); }
#define DSUtil_PlaySound(s)         { if(s) s->Play( 0, 0 ); }
#define DSUtil_PlaySoundLooping(s)  { if(s) s->Play( 0, DSBPLAY_LOOPING ); }

// 3D Listener properties
struct sound_listener_properties
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 forward;
	D3DXVECTOR3 up;
};


//-----------------------------------------------------------------------------
// Name: class CSoundManager
// Desc: 
//-----------------------------------------------------------------------------
class CSoundManager
{
protected:
    LPDIRECTSOUND8 m_pDS;
	LPDIRECTSOUND3DLISTENER				m_pDSListener;
	LPDIRECTSOUND3DBUFFER				m_p3DBuffer;			// Shared and used by EAX and ZoomFX objects
	sound_listener_properties			m_ListenerProperties;
	
	// EAX and ZoomFX support flags
	DWORD								m_dwHardwareSupport;

	// EAX listener object
	CListenerEffectEAX*					m_pEAXListener;
	// EAX Manager object
	LPEAXMANAGER						m_pEAXManager;

public:
    CSoundManager();
    ~CSoundManager();

    HRESULT Initialize( HWND hWnd, DWORD dwCoopLevel );
    inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
    HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );
	HRESULT UpdateListener( D3DXVECTOR3 vPos, D3DXVECTOR3 vForward, D3DXVECTOR3 vUp );
	HRESULT UpdateSource( CSound* pSound, char* szPresetName = NULL );
	DWORD GetHardwareSupport( void ) { return m_dwHardwareSupport; }

	// EAX and ZoomFX functions
	HRESULT InitializeEffects( void );
	HRESULT LoadEAXLevelData( const char* sLevelName );
	HRESULT InitializeListenerAttributes( D3DXVECTOR3 vPos, D3DXVECTOR3 vForward, D3DXVECTOR3 vUp );
	void GetEnvironmentPresetString(TCHAR* sPreset);

    HRESULT Create( CSound** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1, BOOL bZoomFXSupport = FALSE );
};

#endif // CSOUNDMANAGER_H