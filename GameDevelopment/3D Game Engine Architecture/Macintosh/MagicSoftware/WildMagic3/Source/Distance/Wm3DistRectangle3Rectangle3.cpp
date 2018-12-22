// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistRectangle3Rectangle3.h"
#include "Wm3DistSegment3Rectangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistRectangle3Rectangle3<Real>::DistRectangle3Rectangle3 (
    const Rectangle3<Real>& rkRectangle0,
    const Rectangle3<Real>& rkRectangle1)
    :
    m_rkRectangle0(rkRectangle0),
    m_rkRectangle1(rkRectangle1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistRectangle3Rectangle3<Real>::GetRectangle0 () const
{
    return m_rkRectangle0;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistRectangle3Rectangle3<Real>::GetRectangle1 () const
{
    return m_rkRectangle1;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::GetSquared ()
{
    // compare edges of rectangle0 to the interior of rectangle1
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    Segment3<Real> kEdge;
    int i0, i1;
    for (i1 = 0; i1 < 2; i1++)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            kEdge.Origin = m_rkRectangle0.Center +
                (i0*m_rkRectangle0.Extent[1-i1]) *
                m_rkRectangle0.Axis[1-i1];
            kEdge.Direction = m_rkRectangle0.Axis[i1];
            kEdge.Extent = m_rkRectangle0.Extent[i1];
            DistSegment3Rectangle3<Real> kSRDist(kEdge,m_rkRectangle1);
            fSqrDistTmp = kSRDist.GetSquared();
            if ( fSqrDistTmp < fSqrDist )
            {
                m_kClosestPoint0 = kSRDist.GetClosestPoint0();
                m_kClosestPoint1 = kSRDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }

    // compare edges of rectangle1 to the interior of rectangle0
    for (i1 = 0; i1 < 2; i1++)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            kEdge.Origin = m_rkRectangle1.Center +
                (i0*m_rkRectangle1.Extent[1-i1]) *
                m_rkRectangle1.Axis[1-i1];
            kEdge.Direction = m_rkRectangle1.Axis[i1];
            kEdge.Extent = m_rkRectangle1.Extent[i1];
            DistSegment3Rectangle3<Real> kSRDist(kEdge,m_rkRectangle0);
            fSqrDistTmp = kSRDist.GetSquared();
            if ( fSqrDistTmp < fSqrDist )
            {
                m_kClosestPoint0 = kSRDist.GetClosestPoint0();
                m_kClosestPoint1 = kSRDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMCenter0 = m_rkRectangle0.Center + fT*rkVelocity0;
    Vector3<Real> kMCenter1 = m_rkRectangle1.Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle0(kMCenter0,m_rkRectangle0.Axis,
        m_rkRectangle0.Extent);
    Rectangle3<Real> kMRectangle1(kMCenter1,m_rkRectangle1.Axis,
        m_rkRectangle1.Extent);
    return DistRectangle3Rectangle3<Real>(kMRectangle0,kMRectangle1).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMCenter0 = m_rkRectangle0.Center + fT*rkVelocity0;
    Vector3<Real> kMCenter1 = m_rkRectangle1.Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle0(kMCenter0,m_rkRectangle0.Axis,
        m_rkRectangle0.Extent);
    Rectangle3<Real> kMRectangle1(kMCenter1,m_rkRectangle1.Axis,
        m_rkRectangle1.Extent);
    return DistRectangle3Rectangle3<Real>(kMRectangle0,
        kMRectangle1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistRectangle3Rectangle3<float>;

// double
template WM3_ITEM
class DistRectangle3Rectangle3<double>;
//----------------------------------------------------------------------------
}
