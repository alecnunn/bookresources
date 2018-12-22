// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSegment3Box3.h"
#include "Wm3IntrLine3Box3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment3Box3<Real>::IntrSegment3Box3 (const Segment3<Real>& rkSegment,
    const Box3<Real>& rkBox)
    :
    m_rkSegment(rkSegment),
    m_rkBox(rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& IntrSegment3Box3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrSegment3Box3<Real>::GetBox () const
{
    return m_rkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Box3<Real>::Test ()
{
    Real fAWdU[3], fADdU[3], fAWxDdU[3], fRhs;

    Vector3<Real> kDiff = m_rkSegment.Origin - m_rkBox.Center;

    fAWdU[0] = Math<Real>::FAbs(m_rkSegment.Direction.Dot(m_rkBox.Axis[0]));
    fADdU[0] = Math<Real>::FAbs(kDiff.Dot(m_rkBox.Axis[0]));
    fRhs = m_rkBox.Extent[0] + m_rkSegment.Extent*fAWdU[0];
    if ( fADdU[0] > fRhs )
        return false;

    fAWdU[1] = Math<Real>::FAbs(m_rkSegment.Direction.Dot(m_rkBox.Axis[1]));
    fADdU[1] = Math<Real>::FAbs(kDiff.Dot(m_rkBox.Axis[1]));
    fRhs = m_rkBox.Extent[1] + m_rkSegment.Extent*fAWdU[1];
    if ( fADdU[1] > fRhs )
        return false;

    fAWdU[2] = Math<Real>::FAbs(m_rkSegment.Direction.Dot(m_rkBox.Axis[2]));
    fADdU[2] = Math<Real>::FAbs(kDiff.Dot(m_rkBox.Axis[2]));
    fRhs = m_rkBox.Extent[2] + m_rkSegment.Extent*fAWdU[2];
    if ( fADdU[2] > fRhs )
        return false;

    Vector3<Real> kWxD = m_rkSegment.Direction.Cross(kDiff);

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
bool IntrSegment3Box3<Real>::Find ()
{
    Real fT0 = -m_rkSegment.Extent, fT1 = m_rkSegment.Extent;
    return IntrLine3Box3<Real>::DoClipping(fT0,fT1,m_rkSegment.Origin,
        m_rkSegment.Direction,m_rkBox,m_iQuantity,m_akPoint,
        m_iIntersectionType);
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment3Box3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSegment3Box3<Real>::GetPoint (int i) const
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
class IntrSegment3Box3<float>;

// double
template WM3_ITEM
class IntrSegment3Box3<double>;
//----------------------------------------------------------------------------
}
