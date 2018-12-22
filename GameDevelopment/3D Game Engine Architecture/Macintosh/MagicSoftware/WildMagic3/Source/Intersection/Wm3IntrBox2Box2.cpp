// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrBox2Box2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrBox2Box2<Real>::IntrBox2Box2 (const Box2<Real>& rkBox0,
    const Box2<Real>& rkBox1)
    :
    m_rkBox0(rkBox0),
    m_rkBox1(rkBox1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Box2<Real>& IntrBox2Box2<Real>::GetBox0 () const
{
    return m_rkBox0;
}
//----------------------------------------------------------------------------
template <class Real>
const Box2<Real>& IntrBox2Box2<Real>::GetBox1 () const
{
    return m_rkBox1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox2Box2<Real>::Test ()
{
    // convenience variables
    const Vector2<Real>* akA = m_rkBox0.Axis;
    const Vector2<Real>* akB = m_rkBox1.Axis;
    const Real* afEA = m_rkBox0.Extent;
    const Real* afEB = m_rkBox1.Extent;

    // compute difference of box centers, D = C1-C0
    Vector2<Real> kD = m_rkBox1.Center - m_rkBox0.Center;

    Real aafAbsAdB[2][2], fAbsAdD, fRSum;
    
    // axis C0+t*A0
    aafAbsAdB[0][0] = Math<Real>::FAbs(akA[0].Dot(akB[0]));
    aafAbsAdB[0][1] = Math<Real>::FAbs(akA[0].Dot(akB[1]));
    fAbsAdD = Math<Real>::FAbs(akA[0].Dot(kD));
    fRSum = afEA[0] + afEB[0]*aafAbsAdB[0][0] + afEB[1]*aafAbsAdB[0][1];
    if ( fAbsAdD > fRSum )
        return false;

    // axis C0+t*A1
    aafAbsAdB[1][0] = Math<Real>::FAbs(akA[1].Dot(akB[0]));
    aafAbsAdB[1][1] = Math<Real>::FAbs(akA[1].Dot(akB[1]));
    fAbsAdD = Math<Real>::FAbs(akA[1].Dot(kD));
    fRSum = afEA[1] + afEB[0]*aafAbsAdB[1][0] + afEB[1]*aafAbsAdB[1][1];
    if ( fAbsAdD > fRSum )
        return false;

    // axis C0+t*B0
    fAbsAdD = Math<Real>::FAbs(akB[0].Dot(kD));
    fRSum = afEB[0] + afEA[0]*aafAbsAdB[0][0] + afEA[1]*aafAbsAdB[1][0];
    if ( fAbsAdD > fRSum )
        return false;

    // axis C0+t*B1
    fAbsAdD = Math<Real>::FAbs(akB[1].Dot(kD));
    fRSum = afEB[1] + afEA[0]*aafAbsAdB[0][1] + afEA[1]*aafAbsAdB[1][1];
    if ( fAbsAdD > fRSum )
        return false;

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrBox2Box2<float>;

// double
template WM3_ITEM
class IntrBox2Box2<double>;
//----------------------------------------------------------------------------
}
