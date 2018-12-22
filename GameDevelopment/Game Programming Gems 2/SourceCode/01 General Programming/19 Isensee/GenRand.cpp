///////////////////////////////////////////////////////////////////////////////
//
//  GenRand.cpp
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

#include "GenRand.h"

namespace TrueRand
{

///////////////////////////////////////////////////////////////////////////////
//
// Constants
//
// Increase these values to generate larger random input samples

const int kMaxStack       = 1024;       // stack bytes
const int kMaxSoundInput  = 2048;       // sound input buffer size in bytes
const int kMaxMsgs        = 20;         // Win32 messages
const int kMaxWindows     = 20;         // Win32 windows
const int kMaxFiles       = 20;         // temporary files
const int kMaxHardwareRng = 8;          // hardware RNG bytes
const int kMaxSoftwareRng = 8;          // software RNG bytes
const int kMaxCaptureTime = 100;        // mS
const int kMaxAlloc       = 1024;       // maximum temp allocation size

#if !defined(WIN32)
    #include <ctime>
    struct MSG {};
    struct SYSTEM_INFO {};
    struct LARGE_INTEGER {};
    struct MEMORYSTATUS {};
    struct POINT {};
    struct DWORD {};
    struct GUID {};
    struct HWND {};
    struct RECT {};
    struct WIN32_FIND_DATA {};
    struct HANDLE {};
#elif defined(_XBOX)
    struct MSG {};
    struct SYSTEM_INFO {};
#endif

// These values from Intel web site:
// "http://developer.intel.com/design/security/rng/rng.htm"
// "http://developer.intel.com/design/security/rng/rng-capi.pdf"
const int kIntelCryptoType = 22;
const char* skIntelCryptoServer = TEXT( "Intel Hardware Cryptographic Service Provider" );

///////////////////////////////////////////////////////////////////////////////
//
// Local helper class used when enumerating HWNDs

#if defined(WIN32) && !defined(_XBOX)

class WindowEnumData
{
public:

    explicit WindowEnumData( Buffer& randInputs )
    :
        mWindowCount( 0 ),
        mRandInputs( randInputs )
    {
    }

    int GetCount() const
    {
        // Number of HWNDs counted so far
        return mWindowCount;
    }

    void AppendWnd( HWND hWnd )
    {
        RECT Rect;
        USED( GetWindowRect( hWnd, &Rect ) ); // Win32 call
        Append( mRandInputs, hWnd );  // see Buffer.h
        Append( mRandInputs, Rect );
    }

    void Increment()
    {
        ++mWindowCount;
    }

private:

    // disabled
    WindowEnumData();
    WindowEnumData( const WindowEnumData& );
    WindowEnumData& operator=( const WindowEnumData& );

private:

    int mWindowCount;
    Buffer& mRandInputs;
};

#endif

///////////////////////////////////////////////////////////////////////////////
//
// Default ctor

GenRand::GenRand()
:
      mCrypto()
#if defined( TRUERAND_USE_SOUND_INPUT )
    #if defined(WIN32) && !defined(_XBOX)
        , mSndCapture()
        , mSndCaptureBuffer()
        , mSndNotify()
        , mhCaptureDone( NULL )
    #endif
#endif
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Generate a truly random value. Returns a cryptographically secure non-
// reproducible random buffer. The size of the buffer is based on the mixing
// function, currently an MD5 hash, which returns a 16-byte buffer.
// Returns an empty buffer if the function fails.

Buffer GenRand::GetRand()
{
    // Get ourselves a crypto context if we don't have one already
    if( !mCrypto.IsValid() )
    {
        if( !mCrypto.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
            return Buffer();
    }

    // Get uncorrelated random inputs (slow)
    Buffer randInputs = GetRandomInputs();

    // The MD5 hash is the genuinely random value
    return mCrypto.GetHash( CALG_MD5, randInputs );
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns a cryptographically secure non-reproducible random unsigned
// integer.

unsigned int GenRand::GetRandInt()
{
    // Generate a random buffer
    Buffer bufRand = GetRand();

    // "Squeeze" the buffer into an integer
    unsigned int nRand( 0 );
    const unsigned int* pRand = reinterpret_cast< const unsigned int* >
                                ( bufRand.c_str() );
    Buffer::size_type nBlocks = bufRand.length() / sizeof(nRand);
    for( Buffer::size_type i = 0; i < nBlocks; ++i, ++pRand )
        nRand += *pRand;

    return nRand;
}

///////////////////////////////////////////////////////////////////////////////
//
// The maximum size of the random input buffer returned by GetRandomInputs().
// Approximately 10K

Buffer::size_type GenRand::GetMaxRandInputSize() // static
{
    Buffer::size_type nSize( 0 );

    nSize += sizeof(LARGE_INTEGER);
    nSize += kMaxStack;
    nSize += kMaxHardwareRng + kMaxSoftwareRng;

#if defined( TRUERAND_USE_SOUND_INPUT )
    nSize += kMaxSoundInput;
#endif

    nSize += sizeof(MSG) * kMaxMsgs;
    nSize += sizeof(MEMORYSTATUS);
    nSize += sizeof(POINT);
    nSize += sizeof(DWORD) * 4;
    nSize += sizeof(GUID);
    nSize += (sizeof(HWND) + sizeof(RECT)) * kMaxWindows;
    nSize += sizeof(WIN32_FIND_DATA) * kMaxFiles;
    nSize += sizeof(SYSTEM_INFO);
    nSize += sizeof(HANDLE);
    nSize += kMaxAlloc;
    nSize += sizeof(LARGE_INTEGER);

    return nSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Query the current system for random data and concatenate the values into
// a single buffer. Creates a large buffer (approx 10K) of random uncorrelated
// system inputs. The resulting buffer includes the following data:
//
//      Pentium hardware RNG, if available
//      Crypto API RSA random number, if available
//      Up to 20 pending Windows messages
//      Current memory status (load, etc.)
//      Current time at maximum time resolution
//      Current mouse position
//      Disk information( clusters, free space )
//      New GUID, including network card MAC address, if any
//      Window rects of multiple HWNDs
//      File info (names, sizes, times) for multiple temporary files
//      Processor info
//      Temporary kernel object handle
//      Data from the current stack state
//
// Functions that are not widely available on all Windows operating systems
// are avoided to maximize speed. Nevertheless, this function is quite
// slow -- on the order of 100 milliseconds.
//
// Note that the results of some inputs will always be the same on every
// call (e.g. processor info). The advantage of this input is that it
// will be different across machines.

Buffer GenRand::GetRandomInputs()
{
    Buffer randInputs;

    // For speed, preallocate the buffer to the maximum size
    randInputs.reserve( GetMaxRandInputSize() );

    // These can be done in any order. We get the current time at the
    // beginning *and* end since the time spent in this function varies
    // depending on drive and system access times.

    GetCurrTime( randInputs );         // sizeof(LARGE_INTEGER)
    GetStackState( randInputs );       // kMaxStack
    GetHardwareRng( randInputs );      // kMaxHardwareRng + kMaxSoftwareRng
    GetSoundInput( randInputs );       // kMaxSoundInput
    GetPendingMsgs( randInputs );      // sizeof(MSG) * kMaxMsgs
    GetMemoryStatus( randInputs );     // sizeof(MEMORYSTATUS)
    GetCurrMousePos( randInputs );     // sizeof(POINT)
    GetDiskInfo( randInputs );         // sizeof(DWORD) * 4
    GetNetworkMacAddr( randInputs );   // sizeof(GUID)
    GetWindowRects( randInputs );      // (sizeof(HWND) + sizeof(RECT)) * kMaxWindows
    GetTempFileInfo( randInputs );     // sizeof(WIN32_FIND_DATA) * kMaxFiles
    GetCpuInfo( randInputs );          // sizeof(SYSTEM_INFO)
    GetTempHandle( randInputs );       // sizeof(HANDLE)
    GetTempPointer( randInputs );      // kMaxAlloc
    GetCurrTime( randInputs );         // sizeof(LARGE_INTEGER)

    return randInputs;
}

///////////////////////////////////////////////////////////////////////////////
//
// Current stack information

void GenRand::GetStackState( Buffer& randInputs )
{
    unsigned char Stack[ kMaxStack ];
    Append( randInputs, *Stack, kMaxStack );
}

///////////////////////////////////////////////////////////////////////////////
//
// Extract a random number from the Intel chipset hardware, if available.
// Use RSA to generate a cryptographically secure random number.

void GenRand::GetHardwareRng( Buffer& randInputs )
{
    // Get Intel hardware RNG value, if available
    Crypto HardwareRng;
    if( HardwareRng.Acquire( NULL, skIntelCryptoServer, kIntelCryptoType ) )
    {
        Buffer IntelRng;
        if( HardwareRng.GenRandom( kMaxHardwareRng, IntelRng ) )
            randInputs += IntelRng;
    }

    // Get cryptographically secure random number. Unfortunately, no details
    // available from MS on how this value is generated.
    Crypto SoftwareRng;
    if( SoftwareRng.Acquire( NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
    {
        Buffer SoftRng;
        if( SoftwareRng.GenRandom( kMaxSoftwareRng, SoftRng ) )
            randInputs += SoftRng;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Read random data from the default sound input device, if available.

void GenRand::GetSoundInput( Buffer& randInputs )
{
#if defined( TRUERAND_USE_SOUND_INPUT )

    #if defined(WIN32) && !defined(_XBOX)

        // Do the one-time setup of the sound capture interface and sound
        // capture buffer if we haven't done it already
        if( !mSndCapture.IsValid() )
        {
            if( !InitSoundCapture() )
                return;
        }

        // Only one sound capture buffer can be active in the system at any one time
        // for the default capture device, so it's possible that the buffer could 
        // not be created for this instance. In that case, we just bail out.
        if( !mSndCaptureBuffer.IsValid() )
            return;

        // Start capturing sound input from the default input device
        if( !mSndCaptureBuffer.Start( DSCBSTART_LOOPING ) )
            return;

        // Wait for the sound capture buffer to fill up. Typically takes on the 
        // order of a few dozen mS. We wait no longer than kMaxCaptureTime mS.
        // Win32 call
        HANDLE arrHandle[1] = { mhCaptureDone };
        MsgWaitForMultipleObjects( 1, arrHandle, FALSE, kMaxCaptureTime, 0 );

        // Stop capturing input
        mSndCaptureBuffer.Stop();

        // Put the sound input into the random sample buffer
        SoundCaptureBufferLock lock( mSndCaptureBuffer, 0, kMaxSoundInput );
        if( lock.GetPtr1() != NULL )
            Append( randInputs, lock.GetPtr1(), lock.GetBytes1() );

    #else

        // Get platform-specific sound input, e.g. Unix "cat dev/audio | compress"
        USED( randInputs );

    #endif

#else

    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Initialize sound capture

bool GenRand::InitSoundCapture()
{
#if defined( TRUERAND_USE_SOUND_INPUT )

    #if defined(WIN32) && !defined(_XBOX)

        if( !mSndCapture.Create() )
            return false;

        // Determine the capture capabilities
        DSCCAPS sndCaptureCaps;
        sndCaptureCaps.dwSize = sizeof(DSCCAPS);
        if( !mSndCapture.GetCaps( sndCaptureCaps ) )
            return false;

        WAVEFORMATEX waveFormat;
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels  = 1;
        waveFormat.cbSize     = 0;

        // Use a high resolution input format if it's available (22KHz 16-bit),
        // else fall back to standard low resolution format (11KHz 8-bit).
        if( sndCaptureCaps.dwFormats & WAVE_FORMAT_2M16 )
        {
            waveFormat.nSamplesPerSec  = 22050;
            waveFormat.nAvgBytesPerSec = 44100;
            waveFormat.nBlockAlign     = 2;
            waveFormat.wBitsPerSample  = 16; 
        }
        else
        {
            waveFormat.nSamplesPerSec  = 11025;
            waveFormat.nAvgBytesPerSec = 11025;
            waveFormat.nBlockAlign     = 1;
            waveFormat.wBitsPerSample  = 8; 
        }

        // Create the sound input buffer
        DSCBUFFERDESC sndBufferDescription;
        ZeroMemory( &sndBufferDescription, sizeof(sndBufferDescription) );
        sndBufferDescription.dwSize        = sizeof(sndBufferDescription);
        sndBufferDescription.dwBufferBytes = kMaxSoundInput+1;
        sndBufferDescription.lpwfxFormat   = &waveFormat;

        if( !mSndCaptureBuffer.Create( mSndCapture, sndBufferDescription ) )
            return false;

        // Set up a notification position at the end of the buffer so
        // we know when it's been filled during capture
        if( !mSndNotify.Query( mSndCaptureBuffer ) )
            return false;

        // Create the capture done event
        mhCaptureDone = CreateEvent( NULL, FALSE, FALSE, NULL ); // Win32 call
        if( mhCaptureDone == NULL )
            return false;

        DSBPOSITIONNOTIFY arrNotify[1];
        arrNotify[0].dwOffset = kMaxSoundInput;
        arrNotify[0].hEventNotify = mhCaptureDone;
        if( !mSndNotify.SetNotifyPositions( 1, arrNotify ) )
            return false;

    #endif

#endif

    // Assume we're good to capture
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the data of up to kMaxMsgs Windows messages (includes message ID,
// mouse position, HWND, time and message parameters).

void GenRand::GetPendingMsgs( Buffer& randInputs )
{
#if defined(WIN32) && !defined(_XBOX)

    MSG Msg;
    // Win32 call
    for( int i = 0; PeekMessage( &Msg, NULL, 0, 0, PM_NOREMOVE ) != 0 && 
                    i < kMaxMsgs; ++i )
        Append( randInputs, Msg );

#else

    // Get platform-specific pending messages
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Current memory load, total and available memory

void GenRand::GetMemoryStatus( Buffer& randInputs )
{
#if defined(WIN32)

    MEMORYSTATUS MemoryStatus;
    MemoryStatus.dwLength = sizeof(MemoryStatus);
    GlobalMemoryStatus( &MemoryStatus ); // Win32 call
    Append( randInputs, MemoryStatus );

#else

    // Get platform-specific memory status
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Current tick count at the maximum resolution available

void GenRand::GetCurrTime( Buffer& randInputs )
{
#if defined(WIN32)

    LARGE_INTEGER PerfCounter;
    USED( QueryPerformanceCounter( &PerfCounter ) ); // Win32 call
    Append( randInputs, PerfCounter );

#else

    size_t timeT = time( NULL );
    Append( randInputs, timeT );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Current mouse position

void GenRand::GetCurrMousePos( Buffer& randInputs )
{
#if defined(WIN32) && !defined(_XBOX)

    POINT MousePt;
    USED( GetCursorPos( &MousePt ) ); // Win32 call
    Append( randInputs, MousePt );

#else

    // Get platform-specific mouse/joystick position
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Disk information (free clusters, total clusters)

void GenRand::GetDiskInfo( Buffer& randInputs )
{
#if defined(_XBOX)

    // Get statistics on user data region on hard disk
    ULARGE_INTEGER qwFreeBytesAvailable;
    ULARGE_INTEGER qwTotalNumberOfBytes;
    ULARGE_INTEGER qwTotalNumberOfFreeBytes;
    USED( GetDiskFreeSpaceEx( "U:\\", &qwFreeBytesAvailable, 
                              &qwTotalNumberOfBytes,
                              &qwTotalNumberOfFreeBytes ) ); // Win32 call
    Append( randInputs, qwFreeBytesAvailable );
    Append( randInputs, qwTotalNumberOfBytes );
    Append( randInputs, qwTotalNumberOfFreeBytes );

#elif defined(WIN32)

    // We avoid GetDiskFreeSpaceEx() because it's not available on early
    // versions of Win95
    DWORD dwSectorsPerCluster;
    DWORD dwBytesPerSector;
    DWORD dwFreeClusters;
    DWORD dwTotalClusters;
    USED( GetDiskFreeSpace( NULL, &dwSectorsPerCluster, &dwBytesPerSector,
                            &dwFreeClusters, &dwTotalClusters ) ); // Win32 call
    Append( randInputs, dwSectorsPerCluster );
    Append( randInputs, dwBytesPerSector );
    Append( randInputs, dwFreeClusters );
    Append( randInputs, dwTotalClusters );

#else

    // Get platform-specific disk info
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Randomly generated GUID, including network MAC address, if available

void GenRand::GetNetworkMacAddr( Buffer& randInputs )
{
#if defined(WIN32) && !defined(_XBOX)

    // GUIDs always include the network MAC address if the system has
    // a network card installed
    GUID Guid;
    USED( CoCreateGuid( &Guid ) ); // Win32 call
    Append( randInputs, Guid );

#else

    // Get platform-specific network MAC address
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// GetWindowRects() EnumWindows callback. Message parameter is WindowEnumData
// object. Appends window HWNDs and RECTs into the buffer.

#if defined(WIN32) && !defined(_XBOX)
BOOL CALLBACK GetWindowRectProc( HWND hWnd, LPARAM lParam )
{
    WindowEnumData* pWinEnumData = reinterpret_cast< WindowEnumData* >( lParam );
    assert( pWinEnumData != NULL );
    assert( !IsBadWritePtr( pWinEnumData, sizeof(WindowEnumData) ) ); // Win32 call

    // Stop enumerating if we've hit the maximum
    if( pWinEnumData->GetCount() >= kMaxWindows )
        return FALSE;

    // Append window HWND & rect and continue enumerating
    pWinEnumData->AppendWnd( hWnd );
    pWinEnumData->Increment();
    return TRUE;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Get the rect data of up to kMaxWindows current windows

void GenRand::GetWindowRects( Buffer& randInputs )
{
#if defined(WIN32) && !defined(_XBOX)

    WindowEnumData WinEnumData( randInputs );

    // Win32 call
    USED( EnumWindows( GetWindowRectProc, 
                       reinterpret_cast< LPARAM >( &WinEnumData ) ) );

#else

    // Get platform-specific window data
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Information on up to kMaxFiles files in temporary folder (name, times, size)

void GenRand::GetTempFileInfo( Buffer& randInputs )
{
#if defined(WIN32) && !defined(_XBOX)

    // Build a wildcard specification for files in the temp folder
    TCHAR TempFolder[ MAX_PATH ];
    if( GetTempPath( MAX_PATH, TempFolder ) == 0 ) // Win32 call
        return;
    lstrcat( TempFolder, TEXT( "*" ) ); // Win32 call

    // Find the first temp file
    WIN32_FIND_DATA FindData;
    Win32::FindHandle hFind( FindFirstFile( TempFolder, &FindData ) ); // Win32 call

    // Win32 call
    for( int i = 0; FindNextFile( hFind, &FindData ) != 0 && i < kMaxFiles; ++i )
        Append( randInputs, FindData );

#else

    // Get platform-specific temp file info
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Information about the CPU

void GenRand::GetCpuInfo( Buffer& randInputs )
{
#if defined(WIN32) && !defined(_XBOX)

    SYSTEM_INFO SystemInfo;
    GetSystemInfo( &SystemInfo ); // Win32 call
    Append( randInputs, SystemInfo );

#else

    // Get platform-specific CPU info
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Temporary kernel object handle

void GenRand::GetTempHandle( Buffer& randInputs )
{
#if defined(WIN32)

    Handle hEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) ); // Win32 call
    Append( randInputs, hEvent );

#else

    // Get platform-specific temporary kernel object handle
    USED( randInputs );

#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Temporary pointer

void GenRand::GetTempPointer( Buffer& randInputs )
{
    // Generate a pseudo-random allocation size

#if defined(WIN32)

    LARGE_INTEGER PerfCounter;
    USED( QueryPerformanceCounter( &PerfCounter ) ); // Win32 call
    DWORD dwBytes = DWORD( PerfCounter.LowPart % kMaxAlloc );

#else

    size_t dwBytes = size_t( time(NULL) );

#endif

    // Do the allocation
    AutoHeapPtr Heap;
    unsigned char* pAlloc = Heap.Alloc( dwBytes );

    // Include both the pointer and the uninitialized data
    if( pAlloc != NULL )
    {
        Append( randInputs, pAlloc );
        Append( randInputs, *pAlloc, dwBytes );
    }
}

} // end namespace TrueRand

///////////////////////////////////////////////////////////////////////////////
