#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#include <dxerr9.h>
#include <dsound.h>
//#include "DSUtil.h"
#include "DXUtil.h"

#include "CSoundManager.h"

#define LISTENER_POSITION_EPSILON 0.05f
#define LISTENER_ORIENTATION_EPSILON 0.05f

//-----------------------------------------------------------------------------
// Name: CSoundManager::CSoundManager()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
{
    m_pDS = NULL;
	m_pDSListener = NULL;
	m_p3DBuffer = NULL;
	m_dwHardwareSupport = 0;
	m_pEAXListener = NULL;
	m_pEAXManager = NULL;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::~CSoundManager()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
    SAFE_RELEASE( m_pDSListener );
    SAFE_RELEASE( m_pDS ); 
	SAFE_RELEASE( m_p3DBuffer );
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Initialize( HWND  hWnd, 
                                   DWORD dwCoopLevel )
{
    HRESULT             hr;

    SAFE_RELEASE( m_pDS );

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
       return DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
        return DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );   

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSoundManager::InitializeEffects()
// Desc: Queries for EAX and ZoomFX support.  If supported, 
// initializes the EAX listener object.  This function must be 
// called before any others.
// EAX and ZoomFX source effects are initialized as part of creating CSound
// objects.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::InitializeEffects(
	void )
{
	CSound* tempSound = NULL;
	HRESULT	hr;

	// Create a temporary DS buffer so we can check hardware compatibility
	hr = Create( &tempSound, TEXT("BOUNCE.wav"), DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE, GUID_NULL );

	if ( FAILED( hr ) || tempSound == NULL )
		return DXTRACE_ERR( TEXT("CSoundManager::InitializeEffects Failed to create CSound object"), hr );

	if ( FAILED( hr = tempSound->Get3DBufferInterface( 0, &m_p3DBuffer ) ) )
		return DXTRACE_ERR( TEXT("m_pSound->Get3DBufferInterface"), hr );

	//////////////////////////
	// Initialize EAX Manager
	//////////////////////////

	// Retrieve the EAXManagerInterface
	hr = EaxManagerCreate( &m_pEAXManager );

	// Check for possible error
	if ( hr != EM_OK )
	{
		m_pEAXManager = NULL;
		return DXTRACE_ERR( TEXT("EaxManagerCreate"), hr );
	}

	//////////////////////
	// Initialize EAX
	//////////////////////

	m_pEAXListener = new CListenerEffectEAX();
	if ( m_pEAXListener->IsSupported( m_p3DBuffer ) )
	{
		m_dwHardwareSupport |= SND_HDWR_EAX_LISTENER_SUPPORT;
		m_pEAXListener->Initialize( m_p3DBuffer );
		
		// Set default envinroment
		m_pEAXListener->SetEnvironment( m_pEAXManager, "Large Room" );
		m_pDSListener->CommitDeferredSettings();
	}
	else
		delete m_pEAXListener;
	
	////////////////////////////////
	// Check for EAX source support
	////////////////////////////////
	CSourceEffectEAX* m_pEAXSource = new CSourceEffectEAX();
	if ( m_pEAXSource->IsSupported( m_p3DBuffer ) )
		m_dwHardwareSupport |= SND_HDWR_EAX_SOURCE_SUPPORT;
	delete m_pEAXSource;

	////////////////////////////
	// Check for ZoomFX support
	////////////////////////////

	CSourceEffectZoomFX* m_pZoomFXSource = new CSourceEffectZoomFX();
	if ( m_pZoomFXSource->IsSupported( m_p3DBuffer ) )
		m_dwHardwareSupport |= SND_HDWR_ZOOMFX_SOURCE_SUPPORT;
	delete m_pZoomFXSource;
	
	SAFE_RELEASE( m_p3DBuffer );
	SAFE_DELETE( tempSound );

	return S_OK;
}

void CSoundManager::GetEnvironmentPresetString( 
	TCHAR* sPreset )
{
	if ( m_pEAXListener != NULL )
	{
		m_pEAXListener->GetEnvironmentPresetString( sPreset );
	}
	
	return;
}	

//-----------------------------------------------------------------------------
// Name: CSoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       !WARNING! - Setting the primary buffer format and then using this
//                   same dsound object for DirectMusic messes up DirectMusic! 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
HRESULT CSoundManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRL3D|DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR( TEXT("SetFormat"), hr );

	// Demo needs listener interface so let's get it here
	if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)&m_pDSListener ) ) )
    {
        SAFE_RELEASE( pDSBPrimary );
        return DXTRACE_ERR( TEXT("QueryInterface for IID_IDirectSound3DListener"), hr );
    }

    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Get3DListenerInterface()
// Desc: Returns the 3D listener interface associated with primary buffer.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener )
{
    HRESULT             hr;
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( ppDSListener == NULL )
        return E_INVALIDARG;
    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    *ppDSListener = NULL;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)ppDSListener ) ) )
    {
        SAFE_RELEASE( pDSBPrimary );
        return DXTRACE_ERR( TEXT("QueryInterface"), hr );
    }

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Create()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Create( CSound** ppSound, 
                               LPTSTR strWaveFileName, 
                               DWORD dwCreationFlags, 
                               GUID guid3DAlgorithm,
                               DWORD dwNumBuffers,
							   BOOL bZoomFXSupport)
{
    HRESULT hr;
    HRESULT hrRet = S_OK;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    CWaveFile*           pWaveFile      = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( strWaveFileName == NULL || ppSound == NULL || dwNumBuffers < 1 )
        return E_INVALIDARG;

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );

    if( pWaveFile->GetSize() == 0 )
    {
        // Wave is blank, so don't create it.
        hr = E_FAIL;
        goto LFail;
    }

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pWaveFile->GetSize();

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;
    
    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );

    // Be sure to return this error code if it occurs so the
    // callers knows this happened.
    if( hr == DS_NO_VIRTUALIZATION )
        hrRet = DS_NO_VIRTUALIZATION;
            
    if( FAILED(hr) )
    {
        // DSERR_BUFFERTOOSMALL will be returned if the buffer is
        // less than DSBSIZE_FX_MIN and the buffer is created
        // with DSBCAPS_CTRLFX.
        
        // It might also fail if hardware buffer mixing was requested
        // on a device that doesn't support it.
        DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
                    
        goto LFail;
    }

    // Default to use DuplicateSoundBuffer() when created extra buffers since always 
    // create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
    // DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
    if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
    {
        for( i=1; i<dwNumBuffers; i++ )
        {
            if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
            {
                DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), hr );
                goto LFail;
            }
        }
    }
    else
    {
        for( i=1; i<dwNumBuffers; i++ )
        {
            hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
            if( FAILED(hr) )
            {
                DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
                goto LFail;
            }
        }
   }
    
    // Create the sound
    *ppSound = new CSound( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags, m_dwHardwareSupport, bZoomFXSupport );
    
    SAFE_DELETE( apDSBuffer );
    return hrRet;

LFail:
    // Cleanup
    SAFE_DELETE( pWaveFile );
    SAFE_DELETE( apDSBuffer );
    return hr;
}

//-----------------------------------------------------------------------------
// Name: CSoundManager::LoadEAXLevelData(()
// Desc: Loads environmental audio library dataset which is used by EAX Manager
//-----------------------------------------------------------------------------
HRESULT CSoundManager::LoadEAXLevelData( const char* sLevelName )
{
	char sLevel[64] = { '0' };

	if ( m_pEAXManager )
	{
		strcpy( sLevel, sLevelName );

		// Remove the .X file extension from the level name
		int nLength = strlen(sLevel);
		if ( nLength > 0 )
		{
			HRESULT hr;
			sLevel[nLength-1] = '\0';
			strcat( sLevel, "eal" );
			
			// Attempt to load the eal file.
			hr = m_pEAXManager->LoadDataSet( sLevel, 0 );

			// Check for error
			if ( hr != EM_OK )
			{
				return DXTRACE_ERR( TEXT("m_pEAXManager->LoadDataSet"), hr );
			}
			else
				return S_OK;
		}
	}
		
	// Now that level data is loaded, initialize the listener's environment
	//InitializeListenerAttributes();

	return S_FALSE;
}

//-----------------------------------------------------------------------------
// Name: CSoundManager::InitializeListenerAttributes(()
// Desc: Update the listener's attributes
//-----------------------------------------------------------------------------
HRESULT CSoundManager::InitializeListenerAttributes( D3DXVECTOR3 vPos, D3DXVECTOR3 vForward, D3DXVECTOR3 vUp )
{
	LISTENERATTRIBUTES la;
	HRESULT hr;
	
	hr = m_pEAXManager->GetListenerAttributes( &la );

	if ( hr != EM_OK )
	{
		m_pDSListener->SetDistanceFactor( 1.0f, DS3D_IMMEDIATE );
		m_pDSListener->SetRolloffFactor( 1.0f, DS3D_IMMEDIATE );
		return DXTRACE_ERR( TEXT(" m_pEAXManager->GetListenerAttributes"), hr );
	}

	m_pDSListener->SetDistanceFactor(la.fDistanceFactor,DS3D_IMMEDIATE);
	m_pDSListener->SetRolloffFactor(la.fRolloffFactor,DS3D_IMMEDIATE);
	m_pDSListener->SetDopplerFactor(la.fDopplerFactor,DS3D_IMMEDIATE);
	
	// Initialize listener properties
	m_ListenerProperties.position = vPos;
	m_ListenerProperties.forward = vForward;
	m_ListenerProperties.up = vUp;

	m_pDSListener->SetPosition( vPos.x, vPos.y, vPos.z, DS3D_IMMEDIATE );
	m_pDSListener->SetOrientation( vForward.x, vForward.y, vForward.z, vUp.x, vUp.y, vUp.z, DS3D_IMMEDIATE );
	m_pDSListener->SetVelocity( 0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE );

	m_pDSListener->CommitDeferredSettings();

	return S_OK;

}

//-----------------------------------------------------------------------------
// Name: CSoundManager::UpdateListener(()
// Desc: Update the listener's properties based on camera properties
//-----------------------------------------------------------------------------
HRESULT CSoundManager::UpdateListener( D3DXVECTOR3 vPos, D3DXVECTOR3 vForward, D3DXVECTOR3 vUp )
{
	HRESULT hr;
	BOOL bCommit = FALSE;
	
	if ( m_pDSListener == NULL )
	{
		return S_FALSE;
	}

	if ( !d3dxvector3_compare( &m_ListenerProperties.position, &vPos, LISTENER_POSITION_EPSILON ) )
	{
		m_ListenerProperties.position = vPos;
		if ( SUCCEEDED( hr = m_pDSListener->SetPosition( vPos.x, vPos.y, vPos.z, DS3D_DEFERRED ) ) )
			bCommit = TRUE;
		else
			return DXTRACE_ERR( TEXT(" m_pDSListener->SetPosition"), hr );
	}

	if ( !d3dxvector3_compare( &m_ListenerProperties.forward, &vForward, LISTENER_ORIENTATION_EPSILON ) || 
		 !d3dxvector3_compare( &m_ListenerProperties.up, &vUp, LISTENER_ORIENTATION_EPSILON ) )
	{
		m_ListenerProperties.forward = vForward;
		m_ListenerProperties.up = vUp;
		if ( SUCCEEDED( hr = m_pDSListener->SetOrientation( vForward.x, vForward.y, vForward.z, vUp.x, vUp.y, vUp.z, DS3D_DEFERRED ) ) )
			bCommit = TRUE;
		else
			return DXTRACE_ERR( TEXT(" m_pDSListener->SetOrientation"), hr );
	}

	// Now update the EAX listener
	if ( m_pEAXListener != NULL )
		bCommit |= m_pEAXListener->Update( m_pEAXManager, vPos );

	// Only commit if necessary
	if ( bCommit )
	{
		if ( FAILED( hr = m_pDSListener->CommitDeferredSettings() ) )
			return DXTRACE_ERR( TEXT(" m_pDSListener->CommitDeferredSettings"), hr );
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSoundManager::UpdateSource(()
// Desc: Update the source properties based on camera properties
//-----------------------------------------------------------------------------
HRESULT CSoundManager::UpdateSource( CSound* pSound, char* szPresetName )
{
	if ( pSound == NULL )
	{
		return S_FALSE;
	}

	if ( pSound->UpdateEffects( m_pEAXManager, szPresetName ) )
		m_pDSListener->CommitDeferredSettings();

	return S_OK;
}