///////////////////////////////////////////////////////////////////////////////
//
//  GenRand.h
//
//  Generate genuinely random bits
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

#if !defined( TRUERAND_GENRAND_H )
#define TRUERAND_GENRAND_H

// Using sound input for the input sample buffer requires on the order of
// dozens of mS per call to GetRand(). Comment out this #define to remove
// sound input from the sample buffer
#define TRUERAND_USE_SOUND_INPUT

#include "Common.h"
#include "Buffer.h"
#include "Crypto.h"

#if defined( TRUERAND_USE_SOUND_INPUT )
    #include "Sound.h"
#endif

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// True random number generator

class GenRand
{
public:

    GenRand();

    Buffer GetRand();
    unsigned int GetRandInt();

private:

    static Buffer::size_type GetMaxRandInputSize();

    Buffer GetRandomInputs();

    void GetStackState    ( Buffer& );
    void GetHardwareRng   ( Buffer& );
    void GetSoundInput    ( Buffer& );
    void GetPendingMsgs   ( Buffer& );
    void GetMemoryStatus  ( Buffer& );
    void GetCurrTime      ( Buffer& );
    void GetCurrMousePos  ( Buffer& );
    void GetDiskInfo      ( Buffer& );
    void GetNetworkMacAddr( Buffer& );
    void GetWindowRects   ( Buffer& );
    void GetTempFileInfo  ( Buffer& );
    void GetCpuInfo       ( Buffer& );
    void GetTempHandle    ( Buffer& );
    void GetTempPointer   ( Buffer& );

    bool InitSoundCapture();

private:

    // disabled
    GenRand( const GenRand& );
    GenRand& operator=( const GenRand& );

private:

    // For hashing
    Crypto mCrypto;

    // For sound capture on PCs
#if defined( TRUERAND_USE_SOUND_INPUT )
    #if defined(WIN32) && !defined(_XBOX)
        SoundCapture       mSndCapture;
        SoundCaptureBuffer mSndCaptureBuffer;
        SoundNotify        mSndNotify;
        Handle             mhCaptureDone;
    #endif
#endif

};

} // end namespace TrueRand

#endif // TRUERAND_GENRAND_H

///////////////////////////////////////////////////////////////////////////////
