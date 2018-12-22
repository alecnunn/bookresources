///////////////////////////////////////////////////////////////////////////////
//
//  Sound.cpp
//
//  Wrapper for Win32 DirectSound objects
//
//  Copyright © 2001 Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.
//
///////////////////////////////////////////////////////////////////////////////

#include "Sound.h"

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// Create the sound capture interface

bool SoundCapture::Create( const GUID* pGuid )
{
    Release();

    // Win32 call
    HRESULT hr = DirectSoundCaptureCreate( const_cast< GUID* >(pGuid),
                                           &mpCapture, NULL );
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Release the sound capture interface

void SoundCapture::Release()
{
    if( mpCapture != NULL )
    {
        mpCapture->Release(); // Win32 call
        mpCapture = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// TRUE if interface has been created

bool SoundCapture::IsValid() const
{
    return( mpCapture != NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the sound capture capabilities

bool SoundCapture::GetCaps( DSCCAPS& sndCaptureCaps ) const
{
    assert( IsValid() );
    HRESULT hr = mpCapture->GetCaps( &sndCaptureCaps ); // Win32 call
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Access the sound capture interface pointer

const LPDIRECTSOUNDCAPTURE SoundCapture::GetPtr() const
{
    return mpCapture;
}

///////////////////////////////////////////////////////////////////////////////
//
// Create the sound capture buffer

bool SoundCaptureBuffer::Create( const SoundCapture& sndCapture, 
                                 const DSCBUFFERDESC& sndDescription )
{
    Release();
    // Win32 call
    HRESULT hr = IDirectSoundCapture_CreateCaptureBuffer( sndCapture.GetPtr(),
                                                          &sndDescription, 
                                                          &mpCaptureBuffer, NULL );
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Release the sound capture buffer interface

void SoundCaptureBuffer::Release()
{
    if( mpCaptureBuffer != NULL )
    {
        mpCaptureBuffer->Release(); // Win32 call
        mpCaptureBuffer = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// TRUE if interface has been created

bool SoundCaptureBuffer::IsValid() const
{
    return( mpCaptureBuffer != NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Start capture

bool SoundCaptureBuffer::Start( DWORD dwFlags )
{
    assert( IsValid() );
    HRESULT hr = mpCaptureBuffer->Start( dwFlags ); // Win32 call
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Stop capture

bool SoundCaptureBuffer::Stop()
{
    assert( IsValid() );
    HRESULT hr = mpCaptureBuffer->Stop(); // Win32 call
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Lock a portion of the buffer for reading

bool SoundCaptureBuffer::Lock( DWORD dwReadCursor, DWORD dwReadBytes, 
                               VOID** ppAudio1, DWORD* pAudioBytes1, 
                               VOID** ppAudio2, DWORD* pAudioBytes2, DWORD dwFlags )
{
    assert( IsValid() );
    // Win32 call
    HRESULT hr = mpCaptureBuffer->Lock( dwReadCursor, dwReadBytes,
                                        ppAudio1, pAudioBytes1,
                                        ppAudio2, pAudioBytes2, dwFlags );
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Unlock a portion of the buffer

bool SoundCaptureBuffer::Unlock( VOID* pAudio1, DWORD dwAudioBytes1,
             VOID* pAudio2, DWORD dwAudioBytes2 )
{
    assert( IsValid() );
    // Win32 call
    HRESULT hr = mpCaptureBuffer->Unlock( pAudio1, dwAudioBytes1, 
                                          pAudio2, dwAudioBytes2 );
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Access the sound capture buffer interface pointer

const LPDIRECTSOUNDCAPTUREBUFFER SoundCaptureBuffer::GetPtr() const
{
    return mpCaptureBuffer;
}

///////////////////////////////////////////////////////////////////////////////
//
// Create the lock object

SoundCaptureBufferLock::SoundCaptureBufferLock( 
    SoundCaptureBuffer& sndCaptureBuffer, DWORD dwReadCursor, DWORD dwReadBytes, 
    DWORD dwFlags )
:
    mSndCaptureBuffer( sndCaptureBuffer ),
    mpAudio1( NULL ),
    mdwBytes1( 0 ),
    mpAudio2( NULL ),
    mdwBytes2( 0 )
{
    mSndCaptureBuffer.Lock( dwReadCursor, dwReadBytes,
                            reinterpret_cast<VOID**>(&mpAudio1), &mdwBytes1,
                            reinterpret_cast<VOID**>(&mpAudio2), &mdwBytes2,
                            dwFlags );
}

///////////////////////////////////////////////////////////////////////////////
//
// Destroy the lock object

SoundCaptureBufferLock::~SoundCaptureBufferLock()
{
    if( mpAudio1 != NULL )
        mSndCaptureBuffer.Unlock( mpAudio1, mdwBytes1, mpAudio2, mdwBytes2 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Access notification interface from capture buffer

bool SoundNotify::Query( const SoundCaptureBuffer& sndCaptureBuffer )
{
    assert( sndCaptureBuffer.IsValid() );
    Release();
    // Win32 call
    HRESULT hr = sndCaptureBuffer.GetPtr()->QueryInterface
                    ( IID_IDirectSoundNotify, reinterpret_cast<VOID**>( &mpNotify ) );
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Release the notification interface

void SoundNotify::Release()
{
    if( mpNotify != NULL )
    {
        mpNotify->Release(); // Win32 call
        mpNotify = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// TRUE if interfaces has been created

bool SoundNotify::IsValid() const
{
    return( mpNotify != NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the notification positions

bool SoundNotify::SetNotifyPositions( DWORD dwNum, 
                                      const DSBPOSITIONNOTIFY* pNotify )
{
    assert( IsValid() );
    assert( pNotify != NULL );
    // Win32 call
    HRESULT hr = mpNotify->SetNotificationPositions( dwNum, pNotify );
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////
//
// Access notification interface pointer

const LPDIRECTSOUNDNOTIFY SoundNotify::GetPtr() const
{
    return mpNotify;
}

} // end namespace TrueRand

///////////////////////////////////////////////////////////////////////////////
