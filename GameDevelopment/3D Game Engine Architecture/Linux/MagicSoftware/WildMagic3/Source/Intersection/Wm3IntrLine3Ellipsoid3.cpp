// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLine3Ellipsoid3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Ellipsoid3<Real>::IntrLine3Ellipsoid3 (const Line3<Real>& rkLine,
    const Ellipsoid3<Real>& rkEllipsoid)
    :
    m_rkLine(rkLine),
    m_rkEllipsoid(rkEllipsoid)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Ellipsoid3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipsoid3<Real>& IntrLine3Ellipsoid3<Real>::GetEllipsoid () const
{
    return m_rkEllipsoid;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Ellipsoid3<Real>::Test ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the line is X = P+t*D.
    // Substitute the line equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> kM;
    m_rkEllipsoid.GetM(kM);

    Vector3<Real> kDiff = m_rkLine.Origin - m_rkEllipsoid.Center;
    Vector3<Real> kMatDir = kM*m_rkLine.Direction;
    Vector3<Real> kMatDiff = kM*kDiff;
    Real fA2 = m_rkLine.Direction.Dot(kMatDir);
    Real fA1 = m_rkLine.Direction.Dot(kMatDiff);
    Real fA0 = kDiff.Dot(kMatDiff) - (Real)1.0;

    // intersection occurs if Q(t) has real roots
    Real fDiscr = fA1*fA1 - fA0*fA2;
    return fDiscr >= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Ellipsoid3<Real>::Find ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the line is X = P+t*D.
    // Substitute the line equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> kM;
    m_rkEllipsoid.GetM(kM);

    Vector3<Real> kDiff = m_rkLine.Origin - m_rkEllipsoid.Center;
    Vector3<Real> kMatDir = kM*m_rkLine.Direction;
    Vector3<Real> kMatDiff = kM*kDiff;
    Real fA2 = m_rkLine.Direction.Dot(kMatDir);
    Real fA1 = m_rkLine.Direction.Dot(kMatDiff);
    Real fA0 = kDiff.Dot(kMatDiff) - (Real)1.0;

    // intersection occurs if Q(t) has real roots
    Real fDiscr = fA1*fA1 - fA0*fA2;
    Real afT[2];
    if ( fDiscr < (Real)0.0 )
    {
        m_iIntersectionType = IT_EMPTY;
        m_iQuantity = 0;
    }
    else if ( fDiscr > Math<Real>::ZERO_TOLERANCE )
    {
        m_iIntersectionType = IT_SEGMENT;
        m_iQuantity = 2;

        Real fRoot = Math<Real>::Sqrt(fDiscr);
        Real fInvA = ((Real)1.0)/fA2;
        afT[0] = (-fA1 - fRoot)*fInvA;
        afT[1] = (-fA1 + fRoot)*fInvA;
        m_akPoint[0] = m_rkLine.Origin + afT[0]*m_rkLine.Direction;
        m_akPoint[1] = m_rkLine.Origin + afT[1]*m_rkLine.Direction;
    }
    else
    {
        m_iIntersectionType = IT_POINT;
        m_iQuantity = 1;

        afT[0] = -fA1/fA2;
        m_akPoint[0] = m_rkLine.Origin + afT[0]*m_rkLine.Direction;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Ellipsoid3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Ellipsoid3<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrLine3Ellipsoid3<float>;

// double
template WM3_ITEM
class IntrLine3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
