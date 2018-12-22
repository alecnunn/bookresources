//----------------------------------------------------------------------------
// File: soundsystem.hpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#include "soundsystem.hpp"
#include "util.hpp"
#include <stdio.h>
#include <dxerr9.h>
#include <ObjBase.h>

//----------------------------------------------------------------------------
SoundSystem* SoundSystem::spSoundSystem = NULL;

//----------------------------------------------------------------------------
void SoundSystem::Initialize( HWND hWnd )
{
    ASSERT( NULL == spSoundSystem );
    spSoundSystem = new SoundSystem( hWnd );
}

//----------------------------------------------------------------------------
void SoundSystem::Terminate( void )
{
    if ( NULL != spSoundSystem )
    {
        delete spSoundSystem;
        spSoundSystem = NULL;
    }
}    

//----------------------------------------------------------------------------
SoundSystem::SoundSystem( HWND hWnd )
    :
    mpDirectSound( NULL ),
    mpPrimaryDirectSoundBuffer( NULL ),
    mpDirectSound3DListener( NULL )
{
    // Initialize COM
    HRESULT hr; 
    hr = CoInitialize( NULL );
    ASSERT( SUCCEEDED( hr ) );
    
    // Initialize DirectSound
    hr = DirectSoundCreate8( NULL, &mpDirectSound, NULL );
    ASSERT( SUCCEEDED( hr ) );
    
    // Set coop level so we can do things
    hr = mpDirectSound->SetCooperativeLevel( hWnd, DSSCL_PRIORITY );
    ASSERT( SUCCEEDED( hr ) );
    
    // Prepare the primary DirectSoundBuffer
    DSBUFFERDESC dsbd;
    ::memset( &dsbd, 0, sizeof( dsbd ) );
    dsbd.dwSize = sizeof( dsbd );
    dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat = NULL;
    hr = mpDirectSound->CreateSoundBuffer( &dsbd, &mpPrimaryDirectSoundBuffer, NULL );
    ASSERT( SUCCEEDED( hr ) );
    
    // Get the listener interface
    hr = mpPrimaryDirectSoundBuffer->QueryInterface( IID_IDirectSound3DListener, ( void** )&mpDirectSound3DListener );
    ASSERT( SUCCEEDED( hr ) );
    
    // Set the listener orientation
    hr = mpDirectSound3DListener->SetOrientation( 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, DS3D_IMMEDIATE );
    ASSERT( SUCCEEDED( hr ) );
}

//----------------------------------------------------------------------------
SoundSystem::~SoundSystem( void )
{
    if ( NULL != mpDirectSound3DListener )
    {
        mpDirectSound3DListener->Release( );
        mpDirectSound3DListener = NULL;
    }

    if ( NULL != mpPrimaryDirectSoundBuffer )
    {
        mpPrimaryDirectSoundBuffer->Release( );
        mpPrimaryDirectSoundBuffer = NULL;
    }

    if ( NULL != mpDirectSound )
    {
        mpDirectSound->Release( );
        mpDirectSound = NULL;
    }

    CoUninitialize( );
    spSoundSystem = NULL;    
}   

//----------------------------------------------------------------------------
void SoundSystem::CreateDirectSoundBuffer( IDirectSoundBuffer** ppIDirectSoundBuffer, WAVEFORMATEX* pWfx, unsigned int bufferBytes, bool isPositional )
{
    DSBUFFERDESC dsbd;
    dsbd.dwSize = sizeof( dsbd );
    dsbd.guid3DAlgorithm = GUID_NULL;
    dsbd.dwFlags =
        DSBCAPS_GLOBALFOCUS |
        DSBCAPS_CTRLFREQUENCY |
        DSBCAPS_GETCURRENTPOSITION2 |
        DSBCAPS_CTRLVOLUME |
        DSBCAPS_LOCSOFTWARE;
        
    if ( isPositional )        
    {
        dsbd.dwFlags |= DSBCAPS_CTRL3D;
    }
        
    dsbd.dwBufferBytes = bufferBytes;
    dsbd.dwReserved = 0;
    dsbd.lpwfxFormat = pWfx;

    HRESULT hr = mpDirectSound->CreateSoundBuffer( &dsbd, ppIDirectSoundBuffer, NULL );
    ASSERT( SUCCEEDED( hr ) );
}   

//----------------------------------------------------------------------------
void SoundSystem::SetListenerPosition( const D3DVECTOR& pos )
{
    if ( NULL != mpDirectSound3DListener )
    {
        mpDirectSound3DListener->SetPosition( pos.x, pos.y, pos.z, DS3D_IMMEDIATE );
    }
}
//----------------------------------------------------------------------------
void SoundSystem::GetListenerPosition( D3DVECTOR& position ) const
{
    if ( NULL != mpDirectSound3DListener )
    {
        mpDirectSound3DListener->GetPosition( &position );
    }
}
