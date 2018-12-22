///////////////////////////////////////////////////////////////////////////////
//
//  Sound.h
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

#if !defined( TRUERAND_SOUND_H )
#define TRUERAND_SOUND_H

#include "Common.h"

#if defined(WIN32) && !defined(_XBOX)

#include <dsound.h>

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// Wrap IDirectSoundCapture

class SoundCapture
{
public:

    inline SoundCapture() : mpCapture( NULL ) {}
    inline ~SoundCapture() { Release(); }

    bool Create( const GUID* = NULL );
    void Release();
    bool IsValid() const;
    bool GetCaps( DSCCAPS& ) const;
    const LPDIRECTSOUNDCAPTURE GetPtr() const;

private:

    // disabled
    SoundCapture( const SoundCapture& );
    SoundCapture& operator=( const SoundCapture& );

private:

    LPDIRECTSOUNDCAPTURE mpCapture;
    
};

///////////////////////////////////////////////////////////////////////////////
//
// Wrap IDirectSoundCaptureBuffer

class SoundCaptureBuffer
{
public:

    inline SoundCaptureBuffer() : mpCaptureBuffer( NULL ) {}
    inline ~SoundCaptureBuffer() { Release(); }

    bool Create( const SoundCapture&, const DSCBUFFERDESC& );
    void Release();
    bool IsValid() const;
    bool Start( DWORD dwFlags = 0 );
    bool Stop();
    bool Lock( DWORD dwReadCursor, DWORD dwReadBytes, VOID** ppAudio1, 
               DWORD* pAudioBytes1, VOID** ppAudio2 = NULL, 
               DWORD* pAudioBytes2 = NULL, DWORD dwFlags = 0 );
    bool Unlock( VOID* pAudio1, DWORD dwAudioBytes1,
                 VOID* pAudio2 = NULL, DWORD dwAudioBytes2 = 0 );

    const LPDIRECTSOUNDCAPTUREBUFFER GetPtr() const;

private:

    // disabled
    SoundCaptureBuffer( const SoundCaptureBuffer& );
    SoundCaptureBuffer& operator=( const SoundCaptureBuffer& );

private:

    LPDIRECTSOUNDCAPTUREBUFFER mpCaptureBuffer;
    
};

///////////////////////////////////////////////////////////////////////////////
//
// Lock object for the sound capture buffer

class SoundCaptureBufferLock
{
public:

    SoundCaptureBufferLock( SoundCaptureBuffer& sndCaptureBuffer,
                            DWORD dwReadCursor, DWORD dwReadBytes, 
                            DWORD dwFlags = 0 );

    ~SoundCaptureBufferLock();

    const BYTE* GetPtr1() const
    {
        return mpAudio1;
    }

    BYTE* GetPtr1()
    {
        return mpAudio1;
    }

    DWORD GetBytes1() const
    {
        return mdwBytes1;
    }

private:

    // disabled
    SoundCaptureBufferLock();
    SoundCaptureBufferLock( const SoundCaptureBufferLock& );
    SoundCaptureBufferLock& operator=( const SoundCaptureBufferLock& );

private:

    SoundCaptureBuffer& mSndCaptureBuffer;
    BYTE* mpAudio1;
    DWORD mdwBytes1;
    BYTE* mpAudio2;
    DWORD mdwBytes2;
};

///////////////////////////////////////////////////////////////////////////////
//
// Wrap IDirectSoundNotify

class SoundNotify
{
public:

    inline SoundNotify() : mpNotify( NULL ) {}
    inline ~SoundNotify() { Release(); }

    bool Query( const SoundCaptureBuffer& );
    void Release();
    bool IsValid() const;
    bool SetNotifyPositions( DWORD, const DSBPOSITIONNOTIFY* );
    const LPDIRECTSOUNDNOTIFY GetPtr() const;

private:

    // disabled
    SoundNotify( const SoundNotify& );
    SoundNotify& operator=( const SoundNotify& );

private:

    LPDIRECTSOUNDNOTIFY mpNotify;
    
};

} // end namespace TrueRand

#endif // WIN32 && !_XBOX

#endif // TRUERAND_SOUND_H

///////////////////////////////////////////////////////////////////////////////
