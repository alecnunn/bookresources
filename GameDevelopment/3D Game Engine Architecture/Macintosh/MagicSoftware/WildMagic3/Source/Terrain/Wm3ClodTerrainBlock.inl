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
inline unsigned char ClodTerrainBlock::GetX () const
{
    return m_ucX;
}
//----------------------------------------------------------------------------
inline unsigned char ClodTerrainBlock::GetY () const
{
    return m_ucY;
}
//----------------------------------------------------------------------------
inline unsigned char ClodTerrainBlock::GetStride () const
{
    return m_ucStride;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetEven (bool bSet)
{
    if ( bSet )
        m_ucFlags |= EVEN_PARITY;
    else
        m_ucFlags &= ~EVEN_PARITY;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetEven () const
{
    return (m_ucFlags & EVEN_PARITY) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetProcessed (bool bSet)
{
    if ( bSet )
        m_ucFlags |= PROCESSED;
    else
        m_ucFlags &= ~PROCESSED;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetProcessed () const
{
    return (m_ucFlags & PROCESSED) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetVisible (bool bSet)
{
    if ( bSet )
        m_ucFlags |= VISIBLE;
    else
        m_ucFlags &= ~VISIBLE;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetVisible () const
{
    return (m_ucFlags & VISIBLE) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetVisibilityTested (bool bSet)
{
    if ( bSet )
        m_ucFlags |= VISIBILITY_TESTED;
    else
        m_ucFlags &= ~VISIBILITY_TESTED;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetVisibilityTested () const
{
    return (m_ucFlags & VISIBILITY_TESTED) != 0;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::BitsSet () const
{
    return (m_ucFlags & BITS_MASK) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::ClearBits ()
{
    // retain parity bit, all others zeroed out
    m_ucFlags &= EVEN_PARITY;
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDelta (int i) const
{
    // assert( 0 <= i && i <= 4 );
    return m_fDelta[i];
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDeltaMax () const
{
    return m_fDeltaMax;
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDeltaL () const
{
    return m_fDeltaL;
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDeltaH () const
{
    return m_fDeltaH;
}
//----------------------------------------------------------------------------
inline const Vector3f& ClodTerrainBlock::GetMin () const
{
    return m_kMin;
}
//----------------------------------------------------------------------------
inline const Vector3f& ClodTerrainBlock::GetMax () const
{
    return m_kMax;
}
//----------------------------------------------------------------------------
inline unsigned short ClodTerrainBlock::GetParentIndex (
    unsigned short usChild)
{
    // p = (c-1)/4
    return (usChild - 1) >> 2;
}
//----------------------------------------------------------------------------
inline unsigned short ClodTerrainBlock::GetChildIndex (
    unsigned short usParent, unsigned short usIndex)
{
    // c = 4*p+i
    return (usParent << 2) + usIndex;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::IsFirstChild (unsigned short usIndex)
{
    return usIndex > 0 && ((usIndex-1) % 4) == 0;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::IsSibling (unsigned short usIndex,
    unsigned short usTest)
{
    // assert( usIndex > 0 && usTest > 0 );
    return (usIndex-1) >> 2 == (usTest-1) >> 2;
}
//----------------------------------------------------------------------------
