// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef MEMFILE_H
#define MEMFILE_H

#include "PlatformSupport.h"
#include "Wm3Vector3.h"
using namespace Wm3;

// A simple file reader which loads the entire file contents in a buffer.
// By default data is expected to be in big-endian format.

class MemFile
{
public:

    MemFile (const char* acInFileName, bool bBigEndian = true);
    MemFile (const std::string& rkInFileName, bool bBigEndian = true);
    ~MemFile ();
    
    UInt32 ReadUInt32 ();
    UInt16 ReadUInt16 ();
    UInt8 ReadUInt8 ();
    SInt32 ReadSInt32 ();
    SInt16 ReadSInt16 ();
    SInt8 ReadSInt8 ();
    float ReadFloat ();
    std::string ReadString (int* piBytes);
    std::string ReadEvenString (int* piBytes);
    Vector3f ReadVector ();
    bool ReadBytes (UInt8* pucBytes, long lCount);

    void Advance (UInt32 uiBytes);
    void BackUp (UInt32 uiBytes);
    bool DataAvailable () const;
    long FilePos () const;
    long Length () const;    
    bool FileLoaded () const;
    bool BigEndian () const;
    
private:
    bool InitFile (const char* acFileName);
    
    FILE* m_pkFile;
    bool m_bFileLoaded;
    bool m_bBigEndian;
    char* m_pcFileBuffer;
    char* m_pcCurrent;
    long m_lLength;
};

#include "MemFile.inl"

#endif

