#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <assert.h>
#include <dxerr9.h>
#include <dsound.h>
#include "DXUtil.h"

#include "CSound.h"

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZW|D3DFVF_DIFFUSE)

//-----------------------------------------------------------------------------
// Name: CSound::CSound()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSound::CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, 
                DWORD dwNumBuffers, CWaveFile* pWaveFile, DWORD dwCreationFlags,
				DWORD dwHardwareSupport, BOOL bZoomFXSupport)
{
    DWORD i;

	m_fMinDistance = 1.0f;
	m_fMaxDistance = 10.0f;
	m_vPosition = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_pVBMinDistanceSphere = NULL;
	m_pVBMaxDistanceSphere = NULL;
	m_pVBZoomFXBox = NULL;

    m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( NULL != m_apDSBuffer )
    {
        for( i=0; i<dwNumBuffers; i++ )
            m_apDSBuffer[i] = apDSBuffer[i];
    
        m_dwDSBufferSize = dwDSBufferSize;
        m_dwNumBuffers   = dwNumBuffers;
        m_pWaveFile      = pWaveFile;
        m_dwCreationFlags = dwCreationFlags;
        
        FillBufferWithSound( m_apDSBuffer[0], FALSE );
    }

	if ( dwCreationFlags & DSBCAPS_CTRL3D|DSBCAPS_LOCHARDWARE )
	{
		if ( SUCCEEDED( Get3DBufferInterface( 0, &m_pDSBuffer3D ) ) )
		{
			/////////////////////////////////
			// Initialize EAX source effects
			/////////////////////////////////
			if ( dwHardwareSupport & SND_HDWR_EAX_SOURCE_SUPPORT ) // From Sound Manager
			{
				m_pEAXSource = new CSourceEffectEAX();
				m_pEAXSource->Initialize( m_pDSBuffer3D );
			}
			else
				m_pEAXSource = NULL;
			
			///////////////////////
			// Initialize ZoomFX
			///////////////////////

			if ( dwHardwareSupport & SND_HDWR_ZOOMFX_SOURCE_SUPPORT && bZoomFXSupport )	// From Sound Manager
			{
				m_Props.Reset();
				m_pZoomFXSource = new CSourceEffectZoomFX();
				m_pZoomFXSource->Initialize( m_pDSBuffer3D );
			}
			else
				m_pZoomFXSource = NULL;
		}
	}
}




//-----------------------------------------------------------------------------
// Name: CSound::~CSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSound::~CSound()
{
    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        SAFE_RELEASE( m_apDSBuffer[i] ); 
    }

    SAFE_DELETE_ARRAY( m_apDSBuffer ); 
    SAFE_DELETE( m_pWaveFile );

	SAFE_RELEASE( m_pDSBuffer3D );
	SAFE_RELEASE( m_pVBMinDistanceSphere );
	SAFE_RELEASE( m_pVBMaxDistanceSphere );
	SAFE_RELEASE( m_pVBZoomFXBox );
}




//-----------------------------------------------------------------------------
// Name: CSound::FillBufferWithSound()
// Desc: Fills a DirectSound buffer with a sound file 
//-----------------------------------------------------------------------------
HRESULT CSound::FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger )
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if( FAILED( hr = RestoreBuffer( pDSB, NULL ) ) ) 
        return DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    // Lock the buffer down
    if( FAILED( hr = pDSB->Lock( 0, m_dwDSBufferSize, 
                                 &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                 NULL, NULL, 0L ) ) )
        return DXTRACE_ERR( TEXT("Lock"), hr );

    // Reset the wave file to the beginning 
    m_pWaveFile->ResetFile();

    if( FAILED( hr = m_pWaveFile->Read( (BYTE*) pDSLockedBuffer,
                                        dwDSLockedBufferSize, 
                                        &dwWavDataRead ) ) )           
        return DXTRACE_ERR( TEXT("Read"), hr );

    if( dwWavDataRead == 0 )
    {
        // Wav is blank, so just fill with silence
        FillMemory( (BYTE*) pDSLockedBuffer, 
                    dwDSLockedBufferSize, 
                    (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
    }
    else if( dwWavDataRead < dwDSLockedBufferSize )
    {
        // If the wav file was smaller than the DirectSound buffer, 
        // we need to fill the remainder of the buffer with data 
        if( bRepeatWavIfBufferLarger )
        {       
            // Reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
            while( dwReadSoFar < dwDSLockedBufferSize )
            {  
                // This will keep reading in until the buffer is full 
                // for very short files
                if( FAILED( hr = m_pWaveFile->ResetFile() ) )
                    return DXTRACE_ERR( TEXT("ResetFile"), hr );

                hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
                                        dwDSLockedBufferSize - dwReadSoFar,
                                        &dwWavDataRead );
                if( FAILED(hr) )
                    return DXTRACE_ERR( TEXT("Read"), hr );

                dwReadSoFar += dwWavDataRead;
            } 
        }
        else
        {
            // Don't repeat the wav file, just fill in silence 
            FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
                        dwDSLockedBufferSize - dwWavDataRead, 
                        (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        }
    }

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSound::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also NULL if the information is not needed.
//-----------------------------------------------------------------------------
HRESULT CSound::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored )
{
    HRESULT hr;

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
        return DXTRACE_ERR( TEXT("GetStatus"), hr );

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( ( hr = pDSB->Restore() ) == DSERR_BUFFERLOST );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}




//-----------------------------------------------------------------------------
// Name: CSound::GetFreeBuffer()
// Desc: Finding the first buffer that is not playing and return a pointer to 
//       it, or if all are playing return a pointer to a randomly selected buffer.
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer()
{
    if( m_apDSBuffer == NULL )
        return FALSE; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
                break;
        }
    }

    if( i != m_dwNumBuffers )
        return m_apDSBuffer[ i ];
    else
        return m_apDSBuffer[ rand() % m_dwNumBuffers ];
}




//-----------------------------------------------------------------------------
// Name: CSound::GetBuffer()
// Desc: 
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetBuffer( DWORD dwIndex )
{
    if( m_apDSBuffer == NULL )
        return NULL;
    if( dwIndex >= m_dwNumBuffers )
        return NULL;

    return m_apDSBuffer[dwIndex];
}




//-----------------------------------------------------------------------------
// Name: CSound::Get3DBufferInterface()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSound::Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;
    if( dwIndex >= m_dwNumBuffers )
        return E_INVALIDARG;

    *ppDS3DBuffer = NULL;

    return m_apDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)ppDS3DBuffer );
}


//-----------------------------------------------------------------------------
// Name: CSound::Play()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play( DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, LONG lPan )
{
    HRESULT hr;
    BOOL    bRestored;

    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

    if( pDSB == NULL )
        return DXTRACE_ERR( TEXT("GetFreeBuffer"), E_FAIL );

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
        return DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
            return DXTRACE_ERR( TEXT("FillBufferWithSound"), hr );
    }

    if( m_dwCreationFlags & DSBCAPS_CTRLVOLUME )
    {
        pDSB->SetVolume( lVolume );
    }

    if( lFrequency != -1 && 
        (m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY) )
    {
        pDSB->SetFrequency( lFrequency );
    }
    
    if( m_dwCreationFlags & DSBCAPS_CTRLPAN )
    {
        pDSB->SetPan( lPan );
    }
    
    return pDSB->Play( 0, dwPriority, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: CSound::Play3D()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play3D( LPDS3DBUFFER p3DBuffer, DWORD dwPriority, DWORD dwFlags, LONG lFrequency )
{
    HRESULT hr;
    BOOL    bRestored;
    DWORD   dwBaseFrequency;

    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();
    if( pDSB == NULL )
        return DXTRACE_ERR( TEXT("GetFreeBuffer"), E_FAIL );

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
        return DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
            return DXTRACE_ERR( TEXT("FillBufferWithSound"), hr );
    }

    if( m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY )
    {
        pDSB->GetFrequency( &dwBaseFrequency );
        pDSB->SetFrequency( dwBaseFrequency + lFrequency );
    }

    // QI for the 3D buffer 
    LPDIRECTSOUND3DBUFFER pDS3DBuffer;
    hr = pDSB->QueryInterface( IID_IDirectSound3DBuffer, (VOID**) &pDS3DBuffer );
    if( SUCCEEDED( hr ) )
    {
        hr = pDS3DBuffer->SetAllParameters( p3DBuffer, DS3D_IMMEDIATE );
        if( SUCCEEDED( hr ) )
        {
            hr = pDSB->Play( 0, dwPriority, dwFlags );
        }

        pDS3DBuffer->Release();
    }

    return hr;
}



//-----------------------------------------------------------------------------
// Name: CSound::Stop()
// Desc: Stops the sound from playing
//-----------------------------------------------------------------------------
HRESULT CSound::Stop()
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_apDSBuffer[i]->Stop();

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CSound::Reset()
// Desc: Reset all of the sound buffers
//-----------------------------------------------------------------------------
HRESULT CSound::Reset()
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_apDSBuffer[i]->SetCurrentPosition( 0 );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CSound::IsSoundPlaying()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL CSound::IsSoundPlaying()
{
    BOOL bIsPlaying = FALSE;

    if( m_apDSBuffer == NULL )
        return FALSE; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
        }
    }

    return bIsPlaying;
}

//-----------------------------------------------------------------------------
// Name: CSound::SetLocation()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
HRESULT CSound::SetLocation( D3DXVECTOR3 vPos )
{
	HRESULT hr;

	assert( m_pDSBuffer3D != NULL );

	if ( m_pDSBuffer3D != NULL )
	{
		if ( vPos != m_vPosition )
		{
			m_vPosition = vPos;
				
			if ( FAILED( hr= m_pDSBuffer3D->SetPosition( vPos.x, vPos.y, vPos.z, DS3D_IMMEDIATE ) ) )
				return DXTRACE_ERR( TEXT("m_apDSBuffer->SetPosition"), hr );
		}
		return S_OK;
	}
	return S_FALSE;	
}

//-----------------------------------------------------------------------------
// Name: CSound::SetMinMaxDistance
// Desc: Sets the minimum and maximum distance for attenuating volume
//-----------------------------------------------------------------------------
HRESULT CSound::SetMinMaxDistance( float fMin, float fMax )
{
	assert( m_pDSBuffer3D != NULL );

	if ( m_pDSBuffer3D != NULL )
	{
		if ( m_fMinDistance != fMin )
		{
			m_fMinDistance = fMin;
			m_pDSBuffer3D->SetMinDistance( m_fMinDistance, DS3D_IMMEDIATE );
		}

		if ( m_fMaxDistance != fMax )
		{
			m_fMaxDistance = fMax;
			m_pDSBuffer3D->SetMaxDistance( m_fMaxDistance, DS3D_IMMEDIATE );
		}
		return S_OK;
	}
	return S_FALSE;
}

//-----------------------------------------------------------------------------
// Name: CSound::SetZoomFXProperties
// Desc: Sets ZoomFX properties on the DirectSound if supported and enabled
//-----------------------------------------------------------------------------
HRESULT CSound::SetZoomFXProperties( const ZoomFXProperties& props )
{
	m_Props = props;
	
	if ( m_pZoomFXSource != NULL )
		return m_pZoomFXSource->SetProperties( props );
	else
		return S_FALSE;
}

//-----------------------------------------------------------------------------
// Name: CSound::BuildGeometry
// Desc: Build min and max distance spheres
//-----------------------------------------------------------------------------
HRESULT CSound::BuildGeometry( LPDIRECT3DDEVICE9 pD3Ddevice )
{
	CUSTOMVERTEX* vertices;

	vertices = new CUSTOMVERTEX[VERTICES_IN_CIRCLE];
	ZeroMemory( vertices, sizeof(CUSTOMVERTEX)*VERTICES_IN_CIRCLE );

	long lVertexIndex = 0;

	D3DXVECTOR2 points[CIRCLE_SECTIONS+1];

	short segment_index;

	float angle= TWO_PI/CIRCLE_SECTIONS;
	float sine_angle= sin(angle);
	float cosine_angle= cos(angle);

	short point_index;

	//////////////////////////////////////
	// Create sphere for minimum distance
	//////////////////////////////////////

	points[0] = D3DXVECTOR2( (m_fMinDistance*3), 0.0f );

	for (point_index= 0; point_index+1<CIRCLE_SECTIONS; point_index++)
	{
		d3dxvector2_rotate((D3DXVECTOR2 *)&points[point_index], sine_angle, cosine_angle, (D3DXVECTOR2 *)&points[point_index+1]);
	}

	points[CIRCLE_SECTIONS]= points[0];

	// Create sphere for minimum distance
	for (segment_index= 0; segment_index<CIRCLE_SECTIONS; ++segment_index)
	{
		D3DXVECTOR2 *p0= &points[segment_index];
		D3DXVECTOR2 *p1= &points[segment_index+1];
		D3DXVECTOR3 q0, q1;
		D3DXVECTOR3 v0, v1;

		q0 = D3DXVECTOR3( m_vPosition.x+p0->x, m_vPosition.y+p0->y, m_vPosition.z );
		d3dxvector3_to_customvertex( &q0, D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		q1 = D3DXVECTOR3( m_vPosition.x+p1->x, m_vPosition.y+p1->y, m_vPosition.z );
		d3dxvector3_to_customvertex( &q1, D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		q0 = D3DXVECTOR3( m_vPosition.x+p0->y, m_vPosition.y, m_vPosition.z+p0->x );
		d3dxvector3_to_customvertex( &q0, D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		q1 = D3DXVECTOR3( m_vPosition.x+p1->y, m_vPosition.y, m_vPosition.z+p1->x );
		d3dxvector3_to_customvertex( &q1, D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		q0 = D3DXVECTOR3( m_vPosition.x, m_vPosition.y+p0->x, m_vPosition.z+p0->y );
		d3dxvector3_to_customvertex( &q0, D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		q1 = D3DXVECTOR3( m_vPosition.x, m_vPosition.y+p1->x, m_vPosition.z+p1->y );
		d3dxvector3_to_customvertex( &q1, D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
	}

	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all of our custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( pD3Ddevice->CreateVertexBuffer( VERTICES_IN_CIRCLE*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pVBMinDistanceSphere, NULL ) ) )
    {
        return E_FAIL;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
    VOID* pVertices;
    if( FAILED( m_pVBMinDistanceSphere->Lock( 0, sizeof(CUSTOMVERTEX)*VERTICES_IN_CIRCLE, (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, vertices, sizeof(CUSTOMVERTEX)*VERTICES_IN_CIRCLE );
    m_pVBMinDistanceSphere->Unlock();

	delete[] vertices;

	//////////////////////////////////////
	// Create sphere for maximum distance
	//////////////////////////////////////
	{
		CUSTOMVERTEX* max_vertices;
		max_vertices = new CUSTOMVERTEX[VERTICES_IN_CIRCLE];
		ZeroMemory( max_vertices, sizeof(CUSTOMVERTEX)*VERTICES_IN_CIRCLE );

		lVertexIndex = 0;

		ZeroMemory( points, (sizeof(D3DXVECTOR2)*(CIRCLE_SECTIONS+1)) );
		points[0] = D3DXVECTOR2( (m_fMaxDistance/5), 0.0f );

		for (point_index= 0; point_index+1<CIRCLE_SECTIONS; point_index++)
		{
			d3dxvector2_rotate((D3DXVECTOR2 *)&points[point_index], sine_angle, cosine_angle, (D3DXVECTOR2 *)&points[point_index+1]);
		}

		points[CIRCLE_SECTIONS]= points[0];

		// Create sphere for minimum distance
		for (segment_index= 0; segment_index<CIRCLE_SECTIONS; ++segment_index)
		{
			D3DXVECTOR2 *p0= &points[segment_index];
			D3DXVECTOR2 *p1= &points[segment_index+1];
			D3DXVECTOR3 q0, q1;
			D3DXVECTOR3 v0, v1;

			q0 = D3DXVECTOR3( m_vPosition.x+p0->x, m_vPosition.y+p0->y, m_vPosition.z );
			d3dxvector3_to_customvertex( &q0, D3DCOLOR_RGBA(0,0,255,255), &max_vertices[lVertexIndex++] );
			q1 = D3DXVECTOR3( m_vPosition.x+p1->x, m_vPosition.y+p1->y, m_vPosition.z );
			d3dxvector3_to_customvertex( &q1, D3DCOLOR_RGBA(0,0,255,255), &max_vertices[lVertexIndex++] );
			q0 = D3DXVECTOR3( m_vPosition.x+p0->y, m_vPosition.y, m_vPosition.z+p0->x );
			d3dxvector3_to_customvertex( &q0, D3DCOLOR_RGBA(0,0,255,255), &max_vertices[lVertexIndex++] );
			q1 = D3DXVECTOR3( m_vPosition.x+p1->y, m_vPosition.y, m_vPosition.z+p1->x );
			d3dxvector3_to_customvertex( &q1, D3DCOLOR_RGBA(0,0,255,255), &max_vertices[lVertexIndex++] );
			q0 = D3DXVECTOR3( m_vPosition.x, m_vPosition.y+p0->x, m_vPosition.z+p0->y );
			d3dxvector3_to_customvertex( &q0, D3DCOLOR_RGBA(0,0,255,255), &max_vertices[lVertexIndex++] );
			q1 = D3DXVECTOR3( m_vPosition.x, m_vPosition.y+p1->x, m_vPosition.z+p1->y );
			d3dxvector3_to_customvertex( &q1, D3DCOLOR_RGBA(0,0,255,255), &max_vertices[lVertexIndex++] );
		}

		// Create the vertex buffer. Here we are allocating enough memory
		// (from the default pool) to hold all of our custom vertices. We also
		// specify the FVF, so the vertex buffer knows what data it contains.
		if( FAILED( pD3Ddevice->CreateVertexBuffer( VERTICES_IN_CIRCLE*sizeof(CUSTOMVERTEX),
													0, D3DFVF_CUSTOMVERTEX,
													D3DPOOL_DEFAULT, &m_pVBMaxDistanceSphere, NULL ) ) )
		{
			return E_FAIL;
		}

		// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
		// gain access to the vertices. This mechanism is required becuase vertex
		// buffers may be in device memory.
		if( FAILED( m_pVBMaxDistanceSphere->Lock( 0, sizeof(CUSTOMVERTEX)*VERTICES_IN_CIRCLE, (void**)&pVertices, 0 ) ) )
			return E_FAIL;
		memcpy( pVertices, max_vertices, sizeof(CUSTOMVERTEX)*VERTICES_IN_CIRCLE );
		m_pVBMaxDistanceSphere->Unlock();

		delete[] max_vertices;
	}

	//////////////////////////////////////////////////////
	// If this buffer has ZoomFX, create the ZoomFX box 
	//////////////////////////////////////////////////////
	if ( m_pZoomFXSource != NULL )
	{
		D3DXVECTOR3 box_points[8];

		lVertexIndex = 0;
		vertices = new CUSTOMVERTEX[VERTICES_IN_BOX];
		ZeroMemory( vertices, sizeof(CUSTOMVERTEX)*VERTICES_IN_BOX );

		box_points[0] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMax.x, m_vPosition.y + m_Props.box.vMax.y, m_vPosition.z + m_Props.box.vMin.z );
		box_points[1] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMin.x, m_vPosition.y + m_Props.box.vMax.y, m_vPosition.z + m_Props.box.vMin.z );
		box_points[2] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMin.x, m_vPosition.y + m_Props.box.vMin.y, m_vPosition.z + m_Props.box.vMin.z );
		box_points[3] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMax.x, m_vPosition.y + m_Props.box.vMin.y, m_vPosition.z + m_Props.box.vMin.z );
		box_points[4] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMax.x, m_vPosition.y + m_Props.box.vMax.y, m_vPosition.z + m_Props.box.vMax.z );
		box_points[5] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMin.x, m_vPosition.y + m_Props.box.vMax.y, m_vPosition.z + m_Props.box.vMax.z );
		box_points[6] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMin.x, m_vPosition.y + m_Props.box.vMin.y, m_vPosition.z + m_Props.box.vMax.z );
		box_points[7] = D3DXVECTOR3( m_vPosition.x + m_Props.box.vMax.x, m_vPosition.y + m_Props.box.vMin.y, m_vPosition.z + m_Props.box.vMax.z );

		// point0 -> point1
		d3dxvector3_to_customvertex( &box_points[0], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[1], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point1 -> point2
		d3dxvector3_to_customvertex( &box_points[1], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[2], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point2 -> point3
		d3dxvector3_to_customvertex( &box_points[2], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[3], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point3 -> point0
		d3dxvector3_to_customvertex( &box_points[3], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[0], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point4 -> point5
		d3dxvector3_to_customvertex( &box_points[4], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[5], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point5 -> point6
		d3dxvector3_to_customvertex( &box_points[5], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[6], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point6 -> point7
		d3dxvector3_to_customvertex( &box_points[6], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[7], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point7 -> point4
		d3dxvector3_to_customvertex( &box_points[7], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[4], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point1 -> point5
		d3dxvector3_to_customvertex( &box_points[1], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[5], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point2 -> piont6
		d3dxvector3_to_customvertex( &box_points[2], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[6], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point0 -> point4
		d3dxvector3_to_customvertex( &box_points[0], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[4], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// point3 -> point7
		d3dxvector3_to_customvertex( &box_points[3], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );
		d3dxvector3_to_customvertex( &box_points[7], D3DCOLOR_RGBA(255,0,0,255), &vertices[lVertexIndex++] );

		// Create the vertex buffer. Here we are allocating enough memory
		// (from the default pool) to hold all of our custom vertices. We also
		// specify the FVF, so the vertex buffer knows what data it contains.
		if( FAILED( pD3Ddevice->CreateVertexBuffer( VERTICES_IN_BOX*sizeof(CUSTOMVERTEX),
													0, D3DFVF_CUSTOMVERTEX,
													D3DPOOL_DEFAULT, &m_pVBZoomFXBox, NULL ) ) )
		{
			return E_FAIL;
		}

		// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
		// gain access to the vertices. This mechanism is required becuase vertex
		// buffers may be in device memory.
		VOID* pVertices;
		if( FAILED( m_pVBZoomFXBox->Lock( 0, sizeof(CUSTOMVERTEX)*VERTICES_IN_BOX, (void**)&pVertices, 0 ) ) )
			return E_FAIL;
		memcpy( pVertices, vertices, sizeof(CUSTOMVERTEX)*VERTICES_IN_BOX );
		m_pVBZoomFXBox->Unlock();

		delete[] vertices;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSound::Draw
// Desc: Draw spherical and box visuals
//-----------------------------------------------------------------------------
void CSound::Draw( LPDIRECT3DDEVICE9 pD3Ddevice )
{
	if ( m_pVBMinDistanceSphere != NULL )
	{
        D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity( &matWorld );
		pD3Ddevice->SetTransform( D3DTS_WORLD, &matWorld );

		pD3Ddevice->SetStreamSource( 0, m_pVBMinDistanceSphere, 0, sizeof(CUSTOMVERTEX) );
		pD3Ddevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		pD3Ddevice->DrawPrimitive( D3DPT_LINELIST, 0, (VERTICES_IN_CIRCLE/2) );
	}

	if ( m_pVBMaxDistanceSphere != NULL )
	{
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity( &matWorld );
		pD3Ddevice->SetTransform( D3DTS_WORLD, &matWorld );

		pD3Ddevice->SetStreamSource( 0, m_pVBMaxDistanceSphere, 0, sizeof(CUSTOMVERTEX) );
		pD3Ddevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		pD3Ddevice->DrawPrimitive( D3DPT_LINELIST, 0, (VERTICES_IN_CIRCLE/2) );
	}

	if ( m_pVBZoomFXBox != NULL )
	{
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity( &matWorld );
		pD3Ddevice->SetTransform( D3DTS_WORLD, &matWorld );
		
		pD3Ddevice->SetStreamSource( 0, m_pVBZoomFXBox, 0, sizeof(CUSTOMVERTEX) );
		pD3Ddevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		pD3Ddevice->DrawPrimitive( D3DPT_LINELIST, 0, (VERTICES_IN_BOX/2) );
	}
}	

//-----------------------------------------------------------------------------
// Name: CSound::Draw
// Desc: Update EAX source effects 
//-----------------------------------------------------------------------------
BOOL CSound::UpdateEffects( LPEAXMANAGER pEAXManager, char* szPresetName )
{
	if ( m_pEAXSource != NULL )
	{
		return m_pEAXSource->Update( pEAXManager, m_vPosition, szPresetName );
	}

	return FALSE;
}


















