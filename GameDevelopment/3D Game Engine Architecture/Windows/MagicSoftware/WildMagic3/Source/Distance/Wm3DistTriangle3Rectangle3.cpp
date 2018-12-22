// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistTriangle3Rectangle3.h"
#include "Wm3DistSegment3Triangle3.h"
#include "Wm3DistSegment3Rectangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistTriangle3Rectangle3<Real>::DistTriangle3Rectangle3 (
    const Triangle3<Real>& rkTriangle, const Rectangle3<Real>& rkRectangle)
    :
    m_rkTriangle(rkTriangle),
    m_rkRectangle(rkRectangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& DistTriangle3Rectangle3<Real>::GetTriangle () const
{
    return m_rkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistTriangle3Rectangle3<Real>::GetRectangle () const
{
    return m_rkRectangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::GetSquared ()
{
    // compare edges of triangle to the interior of rectangle
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    Segment3<Real> kEdge;
    int i0, i1;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        kEdge.Origin = ((Real)0.5)*(m_rkTriangle.V[i0]+m_rkTriangle.V[i1]);
        kEdge.Direction = m_rkTriangle.V[i1] - m_rkTriangle.V[i0];
        kEdge.Extent = ((Real)0.5)*kEdge.Direction.Normalize();
        DistSegment3Rectangle3<Real> kSRDist(kEdge,m_rkRectangle);
        fSqrDistTmp = kSRDist.GetSquared();
        if ( fSqrDistTmp < fSqrDist )
        {
            m_kClosestPoint0 = kSRDist.GetClosestPoint0();
            m_kClosestPoint1 = kSRDist.GetClosestPoint1();
            fSqrDist = fSqrDistTmp;
        }
    }

    // compare edges of rectangle to the interior of triangle
    for (i1 = 0; i1 < 2; i1++)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            kEdge.Origin = m_rkRectangle.Center +
                (i0*m_rkRectangle.Extent[1-i1]) *
                m_rkRectangle.Axis[1-i1];
            kEdge.Direction = m_rkRectangle.Axis[i1];
            kEdge.Extent = m_rkRectangle.Extent[i1];
            DistSegment3Triangle3<Real> kSTDist(kEdge,m_rkTriangle);
            fSqrDistTmp = kSTDist.GetSquared();
            if ( fSqrDistTmp < fSqrDist )
            {
                m_kClosestPoint0 = kSTDist.GetClosestPoint0();
                m_kClosestPoint1 = kSTDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMV0 = m_rkTriangle.V[0] + fT*rkVelocity0;
    Vector3<Real> kMV1 = m_rkTriangle.V[1] + fT*rkVelocity0;
    Vector3<Real> kMV2 = m_rkTriangle.V[2] + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis,
        m_rkRectangle.Extent);
    return DistTriangle3Rectangle3<Real>(kMTriangle,kMRectangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMV0 = m_rkTriangle.V[0] + fT*rkVelocity0;
    Vector3<Real> kMV1 = m_rkTriangle.V[1] + fT*rkVelocity0;
    Vector3<Real> kMV2 = m_rkTriangle.V[2] + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis,
        m_rkRectangle.Extent);
    return DistTriangle3Rectangle3<Real>(kMTriangle,kMRectangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistTriangle3Rectangle3<float>;

// double
template WM3_ITEM
class DistTriangle3Rectangle3<double>;
//----------------------------------------------------------------------------
}
