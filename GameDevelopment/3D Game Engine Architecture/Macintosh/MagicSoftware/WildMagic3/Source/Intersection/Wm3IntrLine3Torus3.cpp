// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLine3Torus3.h"
#include "Wm3PolynomialRoots.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Torus3<Real>::IntrLine3Torus3 (const Line3<Real>& rkLine,
    const Torus3<Real>& rkTorus)
    :
    m_rkLine(rkLine),
    m_rkTorus(rkTorus)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Torus3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Torus3<Real>& IntrLine3Torus3<Real>::GetTorus () const
{
    return m_rkTorus;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Torus3<Real>::Find ()
{
    // compute coefficients of quartic polynomial
    Real fRo2 = m_rkTorus.Ro*m_rkTorus.Ro;
    Real fRi2 = m_rkTorus.Ri*m_rkTorus.Ri;
    Real fDD = m_rkLine.Direction.Dot(m_rkLine.Direction);
    Real fDE = m_rkLine.Origin.Dot(m_rkLine.Direction);
    Real fVal = m_rkLine.Origin.Dot(m_rkLine.Origin) - (fRo2 + fRi2);

    Polynomial1<Real> kPoly(4);
    kPoly[0] = fVal*fVal - ((Real)4.0)*fRo2*(fRi2 -
        m_rkLine.Origin.Z()*m_rkLine.Origin.Z());
    kPoly[1] = ((Real)4.0)*fDE*fVal +
        ((Real)8.0)*fRo2*m_rkLine.Direction.Z()*m_rkLine.Origin.Z();
    kPoly[2] = ((Real)2.0)*fDD*fVal + ((Real)4.0)*fDE*fDE +
        ((Real)4.0)*fRo2*m_rkLine.Direction.Z()*m_rkLine.Direction.Z();
    kPoly[3] = ((Real)4.0)*fDD*fDE;
    kPoly[4] = fDD*fDD;

    // solve the quartic
    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    kPR.FindB(kPoly,6);
    m_iQuantity = kPR.GetCount();
    const Real* afRoot = kPR.GetRoots();

    // get the intersection points
    for (int i = 0; i < m_iQuantity; i++)
        m_akPoint[i] = m_rkLine.Origin + afRoot[i]*m_rkLine.Direction;

    m_iIntersectionType = ( m_iQuantity > 0 ? IT_POINT : IT_EMPTY );
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Torus3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Torus3<Real>::GetPoint (int i) const
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
class IntrLine3Torus3<float>;

// double
template WM3_ITEM
class IntrLine3Torus3<double>;
//----------------------------------------------------------------------------
}
