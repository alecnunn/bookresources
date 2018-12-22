// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "MemFile.h"

//----------------------------------------------------------------------------
MemFile::MemFile (const char* acInFileName, bool bBigEndian)
{
    m_bBigEndian = bBigEndian;
    m_bFileLoaded = InitFile(acInFileName);
}
//----------------------------------------------------------------------------
MemFile::MemFile (const string& rkInFileName, bool bBigEndian)
{
    m_bBigEndian = bBigEndian;
    m_bFileLoaded = InitFile(rkInFileName.c_str());
}
//----------------------------------------------------------------------------
MemFile::~MemFile ()
{
    delete[] m_pcFileBuffer;
}
//----------------------------------------------------------------------------
UInt32 MemFile::ReadUInt32 ()
{
    if ( !m_bFileLoaded )
        return 0;

    UInt32 uiNumber = *(UInt32*)m_pcCurrent;
    m_pcCurrent += sizeof(UInt32);
    return UInt32ToHost(uiNumber,m_bBigEndian);
}
//----------------------------------------------------------------------------
UInt16 MemFile::ReadUInt16 ()
{
    if ( !m_bFileLoaded )
        return 0;

    UInt16 uiNumber = *(UInt16*)m_pcCurrent;
    m_pcCurrent += sizeof(UInt16);
    return UInt16ToHost(uiNumber,m_bBigEndian);
}
//----------------------------------------------------------------------------
UInt8 MemFile::ReadUInt8 ()
{
    if ( !m_bFileLoaded )
        return 0;

    UInt8 uiNumber = *(UInt8*)m_pcCurrent;
    m_pcCurrent += sizeof(UInt8);
    return uiNumber;
}
//----------------------------------------------------------------------------
SInt32 MemFile::ReadSInt32 ()
{
    if ( !m_bFileLoaded )
        return 0;

    SInt32 iNumber = *(SInt32*)m_pcCurrent;
    m_pcCurrent += sizeof(SInt32);
    return SInt32ToHost(iNumber,m_bBigEndian);
}
//----------------------------------------------------------------------------
SInt16 MemFile::ReadSInt16 ()
{
    if ( !m_bFileLoaded )
        return 0;

    SInt16 iNumber = *(SInt16*)m_pcCurrent;
    m_pcCurrent += sizeof(SInt16);
    return SInt16ToHost(iNumber,m_bBigEndian);
}
//----------------------------------------------------------------------------
SInt8 MemFile::ReadSInt8 ()
{
    if ( !m_bFileLoaded )
        return 0;

    SInt8 iNumber = *(SInt8*)m_pcCurrent;
    m_pcCurrent += sizeof(SInt8);
    return iNumber;
}
//----------------------------------------------------------------------------
float MemFile::ReadFloat ()
{
    if ( !m_bFileLoaded )
        return 0.0f;

    float fNumber = *(float*)m_pcCurrent;
    m_pcCurrent += sizeof(float);
    return FloatToHost(fNumber,m_bBigEndian);
}
//----------------------------------------------------------------------------
string MemFile::ReadString (int* piBytes)
{
    if ( !m_bFileLoaded )
        return string("");

    string kStr;
    *piBytes = 0;
    bool bLoop = true;
    while ( bLoop )
    {
        char cCurrentChar = *m_pcCurrent++;
        ++(*piBytes);
        if ( cCurrentChar && DataAvailable() )
            kStr += cCurrentChar;
        else
            bLoop = false;
    }
    return kStr;
}
//----------------------------------------------------------------------------
string MemFile::ReadEvenString (int* piBytes)
{
    if ( !m_bFileLoaded )
        return string("");

    string kStr;
    *piBytes = 0;
    bool bLoop = true;
    while ( bLoop )
    {
        char cCurrentChar = *m_pcCurrent++;
        ++(*piBytes);
        if ( cCurrentChar && DataAvailable() )
            kStr += cCurrentChar;
        else
        {
            if ( (*piBytes) & 1 )
            {
                ++m_pcCurrent;
                ++(*piBytes);
            }
            bLoop = false;
        }
    }
    return kStr;
}
//----------------------------------------------------------------------------
Vector3f MemFile::ReadVector ()
{
    if ( !m_bFileLoaded )
        return Vector3f::ZERO;

    float fX = ReadFloat();
    float fY = ReadFloat();
    float fZ = ReadFloat();
    return Vector3f(fX,fY,fZ);
}
//----------------------------------------------------------------------------
bool MemFile::ReadBytes (UInt8* pucBytes, long lCount)
{
    if ( !m_bFileLoaded )
        return false;

    memcpy(pucBytes,m_pcCurrent,lCount);
    m_pcCurrent += lCount;
    return true;
}
//----------------------------------------------------------------------------
bool MemFile::InitFile (const char* acFileName)
{
    m_pcFileBuffer = NULL;
    m_pcCurrent = NULL;
    m_lLength = 0;
    
    m_pkFile = fopen(acFileName, "rb");
    if ( !m_pkFile )
        return false;
        
    int iResult = fseek(m_pkFile,0,SEEK_END);
    long lLength = ftell(m_pkFile);
    if ( lLength <= 0 )
    {
        fclose(m_pkFile);
        return false;
    }
    
    iResult = fseek(m_pkFile,0,SEEK_SET);    
    m_pcFileBuffer = new char [lLength];
    if ( !m_pcFileBuffer )
    {
        fclose(m_pkFile);
        return false;
    }
    
    size_t iItems = fread(m_pcFileBuffer,lLength,1,m_pkFile);
    if ( iItems != 1 )
    {
        delete[] m_pcFileBuffer;
        m_pcFileBuffer = NULL;
        fclose(m_pkFile);
        return false;
    }
    
    m_lLength = lLength;
    m_pcCurrent = m_pcFileBuffer;
    fclose(m_pkFile);
    return true;
}
//----------------------------------------------------------------------------

