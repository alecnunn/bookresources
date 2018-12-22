// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ColorRGB.h"
using namespace Wm3;

const ColorRGB ColorRGB::BLACK(0.0f,0.0f,0.0f);
const ColorRGB ColorRGB::WHITE(1.0f,1.0f,1.0f);
const ColorRGB ColorRGB::INVALID(-1.0f,-1.0f,-1.0f);

//----------------------------------------------------------------------------
ColorRGB::ColorRGB ()
{
    m_afTuple[0] = 0.0f;
    m_afTuple[1] = 0.0f;
    m_afTuple[2] = 0.0f;
}
//----------------------------------------------------------------------------
ColorRGB::ColorRGB (float fR, float fG, float fB)
{
    m_afTuple[0] = fR;
    m_afTuple[1] = fG;
    m_afTuple[2] = fB;
}
//----------------------------------------------------------------------------
ColorRGB::ColorRGB (float afTuple[3])
{
    m_afTuple[0] = afTuple[0];
    m_afTuple[1] = afTuple[1];
    m_afTuple[2] = afTuple[2];
}
//----------------------------------------------------------------------------
ColorRGB::ColorRGB (const ColorRGB& rkC)
{
    m_afTuple[0] = rkC.m_afTuple[0];
    m_afTuple[1] = rkC.m_afTuple[1];
    m_afTuple[2] = rkC.m_afTuple[2];
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator= (const ColorRGB& rkC)
{
    m_afTuple[0] = rkC.m_afTuple[0];
    m_afTuple[1] = rkC.m_afTuple[1];
    m_afTuple[2] = rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator== (const ColorRGB& rkC) const
{
    return
        m_afTuple[0] == rkC.m_afTuple[0] &&
        m_afTuple[1] == rkC.m_afTuple[1] &&
        m_afTuple[2] == rkC.m_afTuple[2];
}
//----------------------------------------------------------------------------
bool ColorRGB::operator!= (const ColorRGB& rkC) const
{
    return
        m_afTuple[0] != rkC.m_afTuple[0] ||
        m_afTuple[1] != rkC.m_afTuple[1] ||
        m_afTuple[2] != rkC.m_afTuple[2];
}
//----------------------------------------------------------------------------
int ColorRGB::CompareArrays (const ColorRGB& rkC) const
{
    return memcmp(m_afTuple,rkC.m_afTuple,3*sizeof(float));
}
//----------------------------------------------------------------------------
bool ColorRGB::operator< (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) < 0;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator<= (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) <= 0;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator> (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) > 0;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator>= (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) >= 0;
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator+ (const ColorRGB& rkC) const
{
    return ColorRGB(
        m_afTuple[0] + rkC.m_afTuple[0],
        m_afTuple[1] + rkC.m_afTuple[1],
        m_afTuple[2] + rkC.m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator- (const ColorRGB& rkC) const
{
    return ColorRGB(
        m_afTuple[0] - rkC.m_afTuple[0],
        m_afTuple[1] - rkC.m_afTuple[1],
        m_afTuple[2] - rkC.m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator* (const ColorRGB& rkC) const
{
    return ColorRGB(
        m_afTuple[0]*rkC.m_afTuple[0],
        m_afTuple[1]*rkC.m_afTuple[1],
        m_afTuple[2]*rkC.m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator* (float fScalar) const
{
    return ColorRGB(
        fScalar*m_afTuple[0],
        fScalar*m_afTuple[1],
        fScalar*m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB Wm3::operator* (float fScalar, const ColorRGB& rkC)
{
    return ColorRGB(
        fScalar*rkC.m_afTuple[0],
        fScalar*rkC.m_afTuple[1],
        fScalar*rkC.m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator+= (const ColorRGB& rkC)
{
    m_afTuple[0] += rkC.m_afTuple[0];
    m_afTuple[1] += rkC.m_afTuple[1];
    m_afTuple[2] += rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator-= (const ColorRGB& rkC)
{
    m_afTuple[0] -= rkC.m_afTuple[0];
    m_afTuple[1] -= rkC.m_afTuple[1];
    m_afTuple[2] -= rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator*= (const ColorRGB& rkC)
{
    m_afTuple[0] *= rkC.m_afTuple[0];
    m_afTuple[1] *= rkC.m_afTuple[1];
    m_afTuple[2] *= rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator*= (float fScalar)
{
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    m_afTuple[2] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
void ColorRGB::Clamp ()
{
    for (int i = 0; i < 3; i++)
    {
        if ( m_afTuple[i] > 1.0f )
            m_afTuple[i] = 1.0f;
        else if ( m_afTuple[i] < 0.0f )
            m_afTuple[i] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void ColorRGB::ScaleByMax ()
{
    float fMax = m_afTuple[0];
    if ( m_afTuple[1] > fMax )
        fMax = m_afTuple[1];
    if ( m_afTuple[2] > fMax )
        fMax = m_afTuple[2];

    if ( fMax > 1.0f )
    {
        float fInvMax = 1.0f/fMax;
        for (int i = 0; i < 3; i++)
            m_afTuple[i] *= fInvMax;
    }
}
//----------------------------------------------------------------------------


