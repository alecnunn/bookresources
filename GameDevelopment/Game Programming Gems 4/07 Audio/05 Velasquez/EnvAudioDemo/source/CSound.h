#ifndef CSound_H
#define CSound_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <d3dx9math.h>

// EAX Manager object
#include "eaxman.h"

// Sound effect objects
#include "CSourceEffectEAX.h"
#include "CSourceEffectZoomFX.h"

#include "CWaveFile.h"
#include "math.h"

#define SND_HDWR_EAX_LISTENER_SUPPORT	(1<<0)
#define SND_HDWR_EAX_SOURCE_SUPPORT		(1<<1)
#define SND_HDWR_ZOOMFX_SOURCE_SUPPORT	(1<<2)

//-----------------------------------------------------------------------------
// Name: class CSound
// Desc: Encapsulates functionality of a DirectSound buffer.
//-----------------------------------------------------------------------------
class CSound
{
protected:
    LPDIRECTSOUNDBUFFER*	m_apDSBuffer;
	LPDIRECTSOUND3DBUFFER8	m_pDSBuffer3D;
    DWORD					m_dwDSBufferSize;
    CWaveFile*				m_pWaveFile;
    DWORD					m_dwNumBuffers;
    DWORD					m_dwCreationFlags;
	D3DXVECTOR3				m_vPosition;
	float					m_fMinDistance;	// min and max in terms of DirectSound falloff
	float					m_fMaxDistance;	// min and max in terms of DirectSound falloff

	// Sound effect objects
	CSourceEffectEAX*		m_pEAXSource;
	CSourceEffectZoomFX*	m_pZoomFXSource;

	// Spherical visual representation 
	LPDIRECT3DVERTEXBUFFER9 m_pVBMinDistanceSphere;
	LPDIRECT3DVERTEXBUFFER9 m_pVBMaxDistanceSphere;

	// Box visual representation
	LPDIRECT3DVERTEXBUFFER9 m_pVBZoomFXBox;
	ZoomFXProperties		m_Props;

    HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:
    CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, 
			CWaveFile* pWaveFile, DWORD dwCreationFlags, DWORD dwHardwareSupport, BOOL bZoomFXSupport );
    virtual ~CSound();

    HRESULT Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
    HRESULT FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );
    LPDIRECTSOUNDBUFFER GetFreeBuffer();
    LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex );
	HRESULT SetLocation( D3DXVECTOR3 vPos );
	HRESULT SetMinMaxDistance( float fMin, float fMax );
	HRESULT SetZoomFXProperties( const ZoomFXProperties& props );
	HRESULT BuildGeometry( LPDIRECT3DDEVICE9 pD3Ddevice );
	void Draw( LPDIRECT3DDEVICE9 pD3Ddevice );
	BOOL UpdateEffects( LPEAXMANAGER pEAXManager, char* szPresetName );

    HRESULT Play( DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lVolume = 0, LONG lFrequency = -1, LONG lPan = 0 );
    HRESULT Play3D( LPDS3DBUFFER p3DBuffer, DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lFrequency = 0 );
    HRESULT Stop();
    HRESULT Reset();
    BOOL    IsSoundPlaying();
};

#endif	// CSound_H