// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
inline bool IsHostBigEndian ()
{
    int iEndianTester = 1;
    char* pcEndianChar = NULL;

    pcEndianChar = (char*)&iEndianTester;
    return !(*pcEndianChar);
}
//----------------------------------------------------------------------------
inline UInt32 UInt32ToBigEndian (UInt32 uiTheNumber)
{
    if ( IsHostBigEndian() )
    {
        return uiTheNumber;
    }
    else
    {
        UInt32 uiNumber = uiTheNumber;
        UInt8* puiBytes = (UInt8*)&uiNumber;
        UInt32 uiResult = (puiBytes[3] << 24) | (puiBytes[2] << 16) |
                          (puiBytes[1] << 8) | puiBytes[0];
        return uiResult;
    }
}
//----------------------------------------------------------------------------
inline UInt32 UInt32ToHost (UInt32 uiTheNumber, bool bBigEndian)
{
    if ( bBigEndian )
    {
        #if (macintosh || __MACOS__ || __APPLECC__)
        return uiTheNumber;
        #else
        UInt32 uiNumber = uiTheNumber;
        UInt8* puiBytes = (UInt8*)&uiNumber;
        UInt32 uiResult = (puiBytes[0] << 24) | (puiBytes[1] << 16) |
                          (puiBytes[2] << 8) | puiBytes[3];
        return uiResult;
        #endif
    }
    else
    {
        #if !(macintosh || __MACOS__ || __APPLECC__)
        return uiTheNumber;
        #else
        UInt32 uiNumber = uiTheNumber;
        UInt8* puiBytes = (UInt8*)&uiNumber;
        UInt32 uiResult = (puiBytes[3] << 24) | (puiBytes[2] << 16) |
                          (puiBytes[1] << 8) | puiBytes[0];
        return uiResult;
        #endif
    }
}
//----------------------------------------------------------------------------
inline UInt16 UInt16ToHost (UInt16 uiTheNumber, bool bBigEndian)
{
    if ( bBigEndian )
    {
        #if (macintosh || __MACOS__ || __APPLECC__)
        return uiTheNumber;
        #else
        UInt16 uiNumber = uiTheNumber;
        UInt8* puiBytes = (UInt8*)&uiNumber;
        UInt16 uiResult = (puiBytes[0] << 8) | puiBytes[1];
        return uiResult;
        #endif
    }
    else
    {
        #if !(macintosh || __MACOS__ || __APPLECC__)
        return uiTheNumber;
        #else
        UInt16 uiNumber = uiTheNumber;
        UInt8* puiBytes = (UInt8*)&uiNumber;
        UInt16 uiResult = (puiBytes[1] << 8) | puiBytes[0];
        return uiResult;
        #endif
    }
}
//----------------------------------------------------------------------------
inline SInt32 SInt32ToHost (SInt32 iTheNumber, bool bBigEndian)
{
    if ( bBigEndian )
    {
        #if (macintosh || __MACOS__ || __APPLECC__)
        return iTheNumber;
        #else
        SInt32 iNumber = iTheNumber;
        SInt8* piBytes = (SInt8*)&iNumber;
        SInt32 iResult = (piBytes[0] << 24) | (piBytes[1] << 16) |
                          (piBytes[2] << 8) | piBytes[3];
        return iResult;
        #endif
    }
    else
    {
        #if !(macintosh || __MACOS__ || __APPLECC__)
        return iTheNumber;
        #else
        SInt32 iNumber = iTheNumber;
        SInt8* piBytes = (SInt8*)&iNumber;
        SInt32 iResult = (piBytes[3] << 24) | (piBytes[2] << 16) |
                          (piBytes[1] << 8) | piBytes[0];
        return iResult;
        #endif
    }
}
//----------------------------------------------------------------------------
inline SInt16 SInt16ToHost (SInt16 iTheNumber, bool bBigEndian)
{
    if ( bBigEndian )
    {
        #if (macintosh || __MACOS__ || __APPLECC__)
        return iTheNumber;
        #else
        SInt16 iNumber = iTheNumber;
        SInt8* piBytes = (SInt8*)&iNumber;
        SInt16 iResult = (piBytes[0] << 8) | piBytes[1];
        return iResult;
        #endif
    }
    else
    {
        #if !(macintosh || __MACOS__ || __APPLECC__)
        return iTheNumber;
        #else
        SInt16 iNumber = iTheNumber;
        SInt8* piBytes = (SInt8*)&iNumber;
        SInt16 iResult = (piBytes[1] << 8) | piBytes[0];
        return iResult;
        #endif
    }
}
//----------------------------------------------------------------------------
inline float FloatToHost (float fTheNumber, bool bBigEndian)
{
    if ( bBigEndian )
    {
        #if (macintosh || __MACOS__ || __APPLECC__)
        return fTheNumber;
        #else
        UInt8* puiBytes = (UInt8*)&fTheNumber;
        UInt8 auiResult[4];
        float* pfPtr = (float*)auiResult;
        auiResult[0] = puiBytes[3];
        auiResult[1] = puiBytes[2];
        auiResult[2] = puiBytes[1];
        auiResult[3] = puiBytes[0];
        return *pfPtr;
        #endif
    }
    else
    {
        #if !(macintosh || __MACOS__ || __APPLECC__)
        return fTheNumber;
        #else
        UInt8* puiBytes = (UInt8*)&fTheNumber;
        UInt8 auiResult[4];
        float* pfPtr = (float*)auiResult;
        auiResult[0] = puiBytes[3];
        auiResult[1] = puiBytes[2];
        auiResult[2] = puiBytes[1];
        auiResult[3] = puiBytes[0];
        return *pfPtr;
        #endif
    }
}
//----------------------------------------------------------------------------
template <class T>
inline void Delete (T*& rpkPointer)
{
    delete rpkPointer;
    rpkPointer = NULL;
}
//----------------------------------------------------------------------------
template <class T>
inline void DeleteArray (T*& rpkPointer)
{
    delete[] rpkPointer;
    rpkPointer = NULL;
}
//----------------------------------------------------------------------------

