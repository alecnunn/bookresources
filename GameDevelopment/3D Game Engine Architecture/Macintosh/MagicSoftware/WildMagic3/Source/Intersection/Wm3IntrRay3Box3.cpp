// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrRay3Box3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Box3<Real>::IntrRay3Box3 (const Ray3<Real>& rkRay,
    const Box3<Real>& rkBox)
    :
    m_rkRay(rkRay),
    m_rkBox(rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Box3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrRay3Box3<Real>::GetBox () const
{
    return m_rkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Box3<Real>::Test ()
{
    Real fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fAWxDdU[3], fRhs;

    Vector3<Real> kDiff = m_rkRay.Origin - m_rkBox.Center;

    fWdU[0] = m_rkRay.Direction.Dot(m_rkBox.Axis[0]);
    fAWdU[0] = Math<Real>::FAbs(fWdU[0]);
    fDdU[0] = kDiff.Dot(m_rkBox.Axis[0]);
    fADdU[0] = Math<Real>::FAbs(fDdU[0]);
    if ( fADdU[0] > m_rkBox.Extent[0] && fDdU[0]*fWdU[0] >= (Real)0.0 )
        return false;

    fWdU[1] = m_rkRay.Direction.Dot(m_rkBox.Axis[1]);
    fAWdU[1] = Math<Real>::FAbs(fWdU[1]);
    fDdU[1] = kDiff.Dot(m_rkBox.Axis[1]);
    fADdU[1] = Math<Real>::FAbs(fDdU[1]);
    if ( fADdU[1] > m_rkBox.Extent[1] && fDdU[1]*fWdU[1] >= (Real)0.0 )
        return false;

    fWdU[2] = m_rkRay.Direction.Dot(m_rkBox.Axis[2]);
    fAWdU[2] = Math<Real>::FAbs(fWdU[2]);
    fDdU[2] = kDiff.Dot(m_rkBox.Axis[2]);
    fADdU[2] = Math<Real>::FAbs(fDdU[2]);
    if ( fADdU[2] > m_rkBox.Extent[2] && fDdU[2]*fWdU[2] >= (Real)0.0 )
        return false;

    Vector3<Real> kWxD = m_rkRay.Direction.Cross(kDiff);

    fAWxDdU[0] = Math<Real>::FAbs(kWxD.Dot(m_rkBox.Axis[0]));
    fRhs = m_rkBox.Extent[1]*fAWdU[2] + m_rkBox.Extent[2]*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = Math<Real>::FAbs(kWxD.Dot(m_rkBox.Axis[1]));
    fRhs = m_rkBox.Extent[0]*fAWdU[2] + m_rkBox.Extent[2]*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = Math<Real>::FAbs(kWxD.Dot(m_rkBox.Axis[2]));
    fRhs = m_rkBox.Extent[0]*fAWdU[1] + m_rkBox.Extent[1]*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Box3<Real>::Find ()
{
    Real fT0 = (Real)0.0, fT1 = Math<Real>::MAX_REAL;
    return IntrLine3Box3<Real>::DoClipping(fT0,fT1,m_rkRay.Origin,
        m_rkRay.Direction,m_rkBox,m_iQuantity,m_akPoint,
        m_iIntersectionType);
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay3Box3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrRay3Box3<Real>::GetPoint (int i) const
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
class IntrRay3Box3<float>;

// double
template WM3_ITEM
class IntrRay3Box3<double>;
//----------------------------------------------------------------------------
}
