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
inline void ClodTerrainVertex::SetDependent (int i,
    ClodTerrainVertex* pkDependent)
{
    assert( 0 <= i && i <= 1 );
    m_akDependent[i] = pkDependent;
}
//----------------------------------------------------------------------------
inline ClodTerrainVertex* ClodTerrainVertex::GetDependent (int i)
{
    assert( 0 <= i && i <= 1 );
    return m_akDependent[i];
}
//----------------------------------------------------------------------------
inline bool ClodTerrainVertex::GetEnabled () const
{
    return m_bEnabled;
}
//----------------------------------------------------------------------------
