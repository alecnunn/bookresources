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
inline void MemFile::Advance (UInt32 uiBytes)
{
    m_pcCurrent += uiBytes;
}
//----------------------------------------------------------------------------
inline void MemFile::BackUp (UInt32 uiBytes)
{
    m_pcCurrent -= uiBytes;
}
//----------------------------------------------------------------------------
inline bool MemFile::DataAvailable () const
{
    return FilePos() < m_lLength;
}
//----------------------------------------------------------------------------
inline long MemFile::FilePos () const
{
    return (long)(m_pcCurrent - m_pcFileBuffer);
}
//----------------------------------------------------------------------------
inline long MemFile::Length () const
{
    return m_lLength;
}
//----------------------------------------------------------------------------
inline bool MemFile::FileLoaded () const
{
    return m_bFileLoaded;
}
//----------------------------------------------------------------------------
inline bool MemFile::BigEndian () const
{
    return m_bBigEndian;
}
//----------------------------------------------------------------------------

