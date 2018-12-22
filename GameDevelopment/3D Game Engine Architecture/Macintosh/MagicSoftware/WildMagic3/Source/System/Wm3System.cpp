// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3System.h"
using namespace Wm3;

// support for Load
#include <sys/stat.h>

// support for GetTime
#ifdef __APPLE__
#include <sys/time.h>
static timeval gs_kInitial;
static bool gs_bInitializedTime = false;
#else
#include <sys/timeb.h>
static long gs_lInitialSec = 0;
static long gs_lInitialUSec = 0;
static bool gs_bInitializedTime = false;
#endif

// support for paths
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

//----------------------------------------------------------------------------
void System::SwapBytes (int iSize, void* pvValue)
{
    // size must be even
    assert( iSize >= 2 && (iSize & 1) == 0 );

    char* acBytes = (char*) pvValue;
    for (int i0 = 0, i1 = iSize-1; i0 < iSize/2; i0++, i1--)
    {
        char cSave = acBytes[i0];
        acBytes[i0] = acBytes[i1];
        acBytes[i1] = cSave;
    }
}
//----------------------------------------------------------------------------
void System::SwapBytes (int iSize, int iQuantity, void* pvValue)
{
    // size must be even
    assert( iSize >= 2 && (iSize & 1) == 0 );

    char* acBytes = (char*) pvValue;
    for (int i = 0; i < iQuantity; i++, acBytes += iSize)
    {
        for (int i0 = 0, i1 = iSize-1; i0 < iSize/2; i0++, i1--)
        {
            char cSave = acBytes[i0];
            acBytes[i0] = acBytes[i1];
            acBytes[i1] = cSave;
        }
    }
}
//----------------------------------------------------------------------------
void System::EndianCopy (int iSize, const void* pvSrc, void* pvDst)
{
    memcpy(pvDst,pvSrc,iSize);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(iSize,pvDst);
#endif
}
//----------------------------------------------------------------------------
void System::EndianCopy (int iSize, int iQuantity, const void* pvSrc,
    void* pvDst)
{
    memcpy(pvDst,pvSrc,iSize*iQuantity);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(iSize,iQuantity,pvDst);
#endif
}
//----------------------------------------------------------------------------
double System::GetTime ()
{
#ifdef __APPLE__
    if ( !gs_bInitializedTime )
    {
        gs_bInitializedTime = true;
        gettimeofday(&gs_kInitial,0);
    }

    struct timeval kCurrent;
    gettimeofday(&kCurrent,0);
    
    struct timeval kDelta;
    timersub(&kCurrent,&gs_kInitial,&kDelta);

    return 0.001*(double)(1000*kDelta.tv_sec + kDelta.tv_usec/1000);
#else
    struct timeb kTB;

    if ( !gs_bInitializedTime )
    {
        gs_bInitializedTime = true;
        ftime(&kTB);
        gs_lInitialSec = (long)kTB.time;
        gs_lInitialUSec = 1000*kTB.millitm;
    }

    ftime(&kTB);
    long lCurrentSec = (long)kTB.time;
    long lCurrentUSec = 1000*kTB.millitm;
    long lDeltaSec = lCurrentSec - gs_lInitialSec;
    long lDeltaUSec = lCurrentUSec -gs_lInitialUSec;
    if ( lDeltaUSec < 0 )
    {
        lDeltaUSec += 1000000;
        lDeltaSec--;
    }

    return 0.001*(double)(1000*lDeltaSec + lDeltaUSec/1000);
#endif
}
//----------------------------------------------------------------------------
bool System::Load (const char* acFilename, char*& racBuffer, int& riSize)
{
    struct stat kStat;
    if ( stat(acFilename,&kStat) != 0 )
    {
        // file does not exist
        racBuffer = 0;
        riSize = 0;
        return false;
    }

    FILE* pkFile = fopen(acFilename,"rb");
    assert( pkFile );
    if ( !pkFile )
    {
        racBuffer = 0;
        riSize = 0;
        return false;
    }

    riSize = kStat.st_size;
    racBuffer = new char[riSize];
    int iRead = (int)fread(racBuffer,sizeof(char),riSize,pkFile);
    if ( fclose(pkFile) != 0 || iRead != riSize )
    {
        assert( false );
        delete[] racBuffer;
        racBuffer = 0;
        riSize = 0;
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool System::Save (const char* acFilename, const char* acBuffer, int iSize)
{
    if ( !acBuffer || iSize <= 0 )
    {
        // The input buffer must exist.  It is not possible to verify that
        // the buffer has the specified number of bytes.
        assert( false );
        return false;
    }

    FILE* pkFile = fopen(acFilename,"wb");
    if ( !pkFile )
        return false;

    int iWrite = (int)fwrite(acBuffer,sizeof(char),iSize,pkFile);
    if ( fclose(pkFile) != 0 || iWrite != iSize )
    {
        assert( false );
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool System::Append (const char* acFilename, char* acBuffer, int iSize)
{
    if ( !acBuffer || iSize <= 0 )
    {
        // The input buffer must exist.  It is not possible to verify that
        // the buffer has the specified number of bytes.
        assert( false );
        return false;
    }

    FILE* pkFile = fopen(acFilename,"ab");
    if ( !pkFile )
        return false;

    int iWrite = (int)fwrite(acBuffer,sizeof(char),iSize,pkFile);
    if ( fclose(pkFile) != 0 || iWrite != iSize )
    {
        assert( false );
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
int System::Read1 (const char* acBuffer, int iQuantity, void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    memcpy(pvData,acBuffer,iQuantity);
    return iQuantity;
}
//----------------------------------------------------------------------------
int System::Read2 (const char* acBuffer, int iQuantity, void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    int iNumBytes = 2*iQuantity;
    memcpy(pvData,acBuffer,iNumBytes);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(2,iQuantity,pvData);
#endif
    return iNumBytes;
}
//----------------------------------------------------------------------------
int System::Read4 (const char* acBuffer, int iQuantity, void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    int iNumBytes = 4*iQuantity;
    memcpy(pvData,acBuffer,iNumBytes);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(4,iQuantity,pvData);
#endif
    return iNumBytes;
}
//----------------------------------------------------------------------------
int System::Read8 (const char* acBuffer, int iQuantity, void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    int iNumBytes = 8*iQuantity;
    memcpy(pvData,acBuffer,iNumBytes);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(8,iQuantity,pvData);
#endif
    return iNumBytes;
}
//----------------------------------------------------------------------------
int System::Write1 (char* acBuffer, int iQuantity, const void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    memcpy(acBuffer,pvData,iQuantity);
    return iQuantity;
}
//----------------------------------------------------------------------------
int System::Write2 (char* acBuffer, int iQuantity, const void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    int iNumBytes = 2*iQuantity;
    memcpy(acBuffer,pvData,iNumBytes);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(2,iQuantity,acBuffer);
#endif
    return iNumBytes;
}
//----------------------------------------------------------------------------
int System::Write4 (char* acBuffer, int iQuantity, const void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    int iNumBytes = 4*iQuantity;
    memcpy(acBuffer,pvData,iNumBytes);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(4,iQuantity,acBuffer);
#endif
    return iNumBytes;
}
//----------------------------------------------------------------------------
int System::Write8 (char* acBuffer, int iQuantity, const void* pvData)
{
    assert( acBuffer && iQuantity > 0 && pvData );
    int iNumBytes = 8*iQuantity;
    memcpy(acBuffer,pvData,iNumBytes);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(8,iQuantity,acBuffer);
#endif
    return iNumBytes;
}
//----------------------------------------------------------------------------
int System::Read1 (FILE* pkFile, int iQuantity, void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
    fread(pvData,1,iQuantity,pkFile);
    return iQuantity;
}
//----------------------------------------------------------------------------
int System::Read2 (FILE* pkFile, int iQuantity, void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
    fread(pvData,2,iQuantity,pkFile);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(2,iQuantity,pvData);
#endif
    return 2*iQuantity;
}
//----------------------------------------------------------------------------
int System::Read4 (FILE* pkFile, int iQuantity, void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
    fread(pvData,4,iQuantity,pkFile);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(4,iQuantity,pvData);
#endif
    return 4*iQuantity;
}
//----------------------------------------------------------------------------
int System::Read8 (FILE* pkFile, int iQuantity, void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
    fread(pvData,8,iQuantity,pkFile);
#ifdef WM3_BIG_ENDIAN
    SwapBytes(8,iQuantity,pvData);
#endif
    return 8*iQuantity;
}
//----------------------------------------------------------------------------
int System::Write1 (FILE* pkFile, int iQuantity, const void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
    fwrite(pvData,1,iQuantity,pkFile);
    return iQuantity;
}
//----------------------------------------------------------------------------
int System::Write2 (FILE* pkFile, int iQuantity, const void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
#ifdef WM3_BIG_ENDIAN
    const short* psData = (const short*)pvData;
    for (int i = 0; i < iQuantity; i++)
    {
        short sTemp = *psData++;
        SwapBytes(2,&sTemp);
        fwrite(&sTemp,2,1,pkFile);
    }
#else
    fwrite(pvData,2,iQuantity,pkFile);
#endif
    return 2*iQuantity;
}
//----------------------------------------------------------------------------
int System::Write4 (FILE* pkFile, int iQuantity, const void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
#ifdef WM3_BIG_ENDIAN
    const int* piData = (const int*)pvData;
    for (int i = 0; i < iQuantity; i++)
    {
        int iTemp = *piData++;
        SwapBytes(4,&iTemp);
        fwrite(&iTemp,4,1,pkFile);
    }
#else
    fwrite(pvData,4,iQuantity,pkFile);
#endif
    return 4*iQuantity;
}
//----------------------------------------------------------------------------
int System::Write8 (FILE* pkFile, int iQuantity, const void* pvData)
{
    assert( pkFile && iQuantity > 0 && pvData );
#ifdef WM3_BIG_ENDIAN
    const double* pdData = (const double*)pvData;
    for (int i = 0; i < iQuantity; i++)
    {
        double dTemp = *pdData++;
        SwapBytes(8,&dTemp);
        fwrite(&dTemp,8,1,pkFile);
    }
#else
    fwrite(pvData,8,iQuantity,pkFile);
#endif
    return 8*iQuantity;
}
//----------------------------------------------------------------------------
const char* System::GetPath (const char* acFilename)
{
#ifdef __APPLE__
    OSStatus kError;
    FSRef kRef, kParentRef;
    ProcessSerialNumber kPSN;
    UInt8 acGlobalPath[2048];

    MacGetCurrentProcess(&kPSN);
    kError = GetProcessBundleLocation(&kPSN,&kRef);
    kError = FSGetCatalogInfo(&kRef,0,0,0,0,&kParentRef);
    kError = FSRefMakePath(&kParentRef,acGlobalPath,2048);
    kError = chdir(acGlobalPath);
#endif

    return acFilename;
}
//----------------------------------------------------------------------------
unsigned int System::MakeRGB (unsigned char ucR, unsigned char ucG,
    unsigned char ucB)
{
#ifdef WM3_BIG_ENDIAN
    return (0xFF | (ucB << 8) | (ucG << 16) | (ucR << 24));
#else
    return (ucR | (ucG << 8) | (ucB << 16) | (0xFF << 24));
#endif
}
//----------------------------------------------------------------------------
unsigned int System::MakeRGBA (unsigned char ucR, unsigned char ucG,
    unsigned char ucB, unsigned char ucA)
{
#ifdef WM3_BIG_ENDIAN
    return (ucA | (ucB << 8) | (ucG << 16) | (ucR << 24));
#else
    return (ucR | (ucG << 8) | (ucB << 16) | (ucA << 24));
#endif
}
//----------------------------------------------------------------------------
