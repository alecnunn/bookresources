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
inline ColorRGB::operator const float* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline ColorRGB::operator float* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline float ColorRGB::operator[] (int i) const
{
    assert( 0 <= i && i <= 2 );
    if ( i < 0 )
        i = 0;
    else if ( i > 2 )
        i = 2;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline float& ColorRGB::operator[] (int i)
{
    assert( 0 <= i && i <= 2 );
    if ( i < 0 )
        i = 0;
    else if ( i > 2 )
        i = 2;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline float ColorRGB::R () const
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
inline float& ColorRGB::R ()
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
inline float ColorRGB::G () const
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
inline float& ColorRGB::G ()
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
inline float ColorRGB::B () const
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
inline float& ColorRGB::B ()
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
