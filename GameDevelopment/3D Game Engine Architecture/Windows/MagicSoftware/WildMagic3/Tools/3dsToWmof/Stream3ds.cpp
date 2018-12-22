// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Stream3ds.h"

//----------------------------------------------------------------------------
Stream3ds::Stream3ds ()
{
    m_acBuffer = 0;
    m_iBufSize = 0;
    m_iPos = 0;
}
//----------------------------------------------------------------------------
Stream3ds::~Stream3ds ()
{
    delete[] m_acBuffer;
}
//----------------------------------------------------------------------------
bool Stream3ds::Load (const char* acFilename)
{
    // if re-loading, free buffer
    delete[] m_acBuffer;

    if ( !System::Load(acFilename,m_acBuffer,m_iBufSize) )
        return false;

    m_iPos = 0;
    return true;
}
//----------------------------------------------------------------------------
void Stream3ds::SetRelativePos (int iPos)
{
    // Sets the position of the file relative to the current position.
    // If the new position is invalid, then it sets it to the end of the file.
    // Readers that use this class will most likely loop until the position
    // gets to a certain point.  Thus, by setting it to the end of the file,
    // it ensures that all those loops will terminate and it will hopefully
    // exit gracefully.

    m_iPos += iPos;
    if ( InvalidPos() )
        m_iPos = m_iBufSize;
}
//----------------------------------------------------------------------------
void Stream3ds::SetAbsolutePos (int iPos)
{
    // Sets the absolute position in the file.  If the new position is
    // invalid, then it sets it to the end of the file.
    m_iPos = iPos;
    if ( InvalidPos() )
        m_iPos = m_iBufSize;
}
//----------------------------------------------------------------------------
bool Stream3ds::InvalidPos ()
{
    return ( !m_acBuffer || m_iPos < 0 || m_iPos >= m_iBufSize );
}
//----------------------------------------------------------------------------
int Stream3ds::GetPos ()
{
    return m_iPos;
}
//----------------------------------------------------------------------------
int Stream3ds::GetMaxPos ()
{
    return m_iBufSize;
}
//----------------------------------------------------------------------------
char Stream3ds::ReadChar ()
{
    return m_acBuffer[m_iPos++];
}
//----------------------------------------------------------------------------
String Stream3ds::ReadString ()
{
    if ( InvalidPos() )
        return "";
    
    String kStr = &m_acBuffer[m_iPos];
    m_iPos += kStr.GetLength() + 1;
    return kStr;
}
//----------------------------------------------------------------------------
int Stream3ds::ReadUInt1 ()
{
    if ( InvalidPos() )
        return 0;

    char cInt = ReadChar();
    return (int)((unsigned char)cInt);
}
//----------------------------------------------------------------------------
int Stream3ds::ReadUInt2 ()
{
    if ( InvalidPos() )
        return 0;

    unsigned short* usRet = (unsigned short*)&m_acBuffer[m_iPos];
    m_iPos += 2;
    return (int)(*usRet);
}
//----------------------------------------------------------------------------
int Stream3ds::ReadInt2 ()
{
    if ( InvalidPos() )
        return 0;

    short* usRet = (short*)&m_acBuffer[m_iPos];
    m_iPos += 2;
    return (int)(*usRet);
}
//----------------------------------------------------------------------------
int Stream3ds::ReadInt4 ()
{
    if ( InvalidPos() )
        return 0;

    int* iRet = (int*)&m_acBuffer[m_iPos];
    m_iPos += 4;
    return *iRet;
}
//----------------------------------------------------------------------------
int Stream3ds::ReadChunkPointer ()
{
    if ( InvalidPos() )
        return m_iBufSize;

    // Chunk pointers are relative to the start of the chunk, so after
    // reading an ID (2 bytes) and this pointer (4 bytes) we need to adjust
    // the absolute address backwards by 6 bytes.
    int iChunkPointer = ReadInt4();
    return iChunkPointer+GetPos()-6;
}
//----------------------------------------------------------------------------
float Stream3ds::ReadFloat ()
{
    if ( InvalidPos() )
        return 0;

    float* fRet = (float*)&m_acBuffer[m_iPos];
    m_iPos += 4;
    return *fRet;
}
//----------------------------------------------------------------------------
bool Stream3ds::ReadBool ()
{
    if ( InvalidPos() )
        return false;

    char cRet = m_acBuffer[m_iPos++];
    return (cRet != 0);
}
//----------------------------------------------------------------------------
ColorRGBA Stream3ds::ReadColorFloat ()
{
    if ( InvalidPos() )
        return ColorRGBA::BLACK;

    float fR = ReadFloat();
    float fG = ReadFloat();
    float fB = ReadFloat();
    return ColorRGBA(fR,fG,fB,1.0f);
}
//----------------------------------------------------------------------------
ColorRGBA Stream3ds::ReadColorInt ()
{
    if ( InvalidPos() )
        return ColorRGBA::BLACK;

    int iR = ReadUInt1();
    int iG = ReadUInt1();
    int iB = ReadUInt1();
    return ColorRGBA(iR/255.0f,iG/255.0f,iB/255.0f,1.0f);
}
//----------------------------------------------------------------------------
Matrix3f Stream3ds::ReadMatrix3f ()
{
    if ( InvalidPos() )
        return Matrix3f::IDENTITY;

    Matrix3f kRet;
    for (int i = 0; i < 3; i++)
    {
        kRet[0][i] = ReadFloat();
        kRet[1][i] = ReadFloat();
        kRet[2][i] = ReadFloat();
    }

    return kRet;
}
//----------------------------------------------------------------------------
Vector3f Stream3ds::ReadVector3f ()
{
    if ( InvalidPos() )
        return Vector3f::ZERO;

    float fX = ReadFloat();
    float fY = ReadFloat();
    float fZ = ReadFloat();
    return Vector3f(fX,fY,fZ);
}
//----------------------------------------------------------------------------
Vector2f Stream3ds::ReadVector2f ()
{
    if ( InvalidPos() )
        return Vector2f::ZERO;

    float fX = ReadFloat();
    float fY = ReadFloat();
    return Vector2f(fX,fY);
}
//----------------------------------------------------------------------------
