// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLine3Sphere3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Sphere3<Real>::IntrLine3Sphere3 (const Line3<Real>& rkLine,
    const Sphere3<Real>& rkSphere)
    :
    m_rkLine(rkLine),
    m_rkSphere(rkSphere)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Sphere3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrLine3Sphere3<Real>::GetSphere () const
{
    return m_rkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Sphere3<Real>::Test ()
{
    Vector3<Real> kDiff = m_rkLine.Origin - m_rkSphere.Center;
    Real fA0 = kDiff.Dot(kDiff) - m_rkSphere.Radius*m_rkSphere.Radius;
    Real fA1 = m_rkLine.Direction.Dot(kDiff);
    Real fDiscr = fA1*fA1 - fA0;
    return fDiscr >= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Sphere3<Real>::Find ()
{
    Vector3<Real> kDiff = m_rkLine.Origin - m_rkSphere.Center;
    Real fA0 = kDiff.Dot(kDiff) - m_rkSphere.Radius*m_rkSphere.Radius;
    Real fA1 = m_rkLine.Direction.Dot(kDiff);
    Real fDiscr = fA1*fA1 - fA0;

    if ( fDiscr < (Real)0.0 )
    {
        m_iQuantity = 0;
    }
    else if ( fDiscr >= Math<Real>::ZERO_TOLERANCE )
    {
        Real fRoot = Math<Real>::Sqrt(fDiscr);
        m_afLineT[0] = -fA1 - fRoot;
        m_afLineT[1] = -fA1 + fRoot;
        m_akPoint[0] = m_rkLine.Origin + m_afLineT[0]*m_rkLine.Direction;
        m_akPoint[1] = m_rkLine.Origin + m_afLineT[1]*m_rkLine.Direction;
        m_iQuantity = 2;
    }
    else
    {
        m_afLineT[0] = -fA1;
        m_akPoint[0] = m_rkLine.Origin + m_afLineT[0]*m_rkLine.Direction;
        m_iQuantity = 1;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Sphere3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Sphere3<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLine3Sphere3<Real>::GetLineT (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_afLineT[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrLine3Sphere3<float>;

// double
template WM3_ITEM
class IntrLine3Sphere3<double>;
//----------------------------------------------------------------------------
}
