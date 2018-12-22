// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistTriangle3Triangle3.h"
#include "Wm3DistSegment3Triangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistTriangle3Triangle3<Real>::DistTriangle3Triangle3 (
    const Triangle3<Real>& rkTriangle0, const Triangle3<Real>& rkTriangle1)
    :
    m_rkTriangle0(rkTriangle0),
    m_rkTriangle1(rkTriangle1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& DistTriangle3Triangle3<Real>::GetTriangle0 () const
{
    return m_rkTriangle0;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& DistTriangle3Triangle3<Real>::GetTriangle1 () const
{
    return m_rkTriangle1;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Triangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Triangle3<Real>::GetSquared ()
{
    // compare edges of triangle0 to the interior of triangle1
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    Segment3<Real> kEdge;
    int i0, i1;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        kEdge.Origin = ((Real)0.5)*(m_rkTriangle0.V[i0]+m_rkTriangle0.V[i1]);
        kEdge.Direction = m_rkTriangle0.V[i1] - m_rkTriangle0.V[i0];
        kEdge.Extent = ((Real)0.5)*kEdge.Direction.Normalize();
        DistSegment3Triangle3<Real> kSTDist(kEdge,m_rkTriangle1);
        fSqrDistTmp = kSTDist.GetSquared();
        if ( fSqrDistTmp < fSqrDist )
        {
            m_kClosestPoint0 = kSTDist.GetClosestPoint0();
            m_kClosestPoint1 = kSTDist.GetClosestPoint1();
            fSqrDist = fSqrDistTmp;
        }
    }

    // compare edges of triangle1 to the interior of triangle0
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        kEdge.Origin = ((Real)0.5)*(m_rkTriangle1.V[i0]+m_rkTriangle1.V[i1]);
        kEdge.Direction = m_rkTriangle1.V[i1] - m_rkTriangle1.V[i0];
        kEdge.Extent = ((Real)0.5)*kEdge.Direction.Normalize();
        DistSegment3Triangle3<Real> kSTDist(kEdge,m_rkTriangle0);
        fSqrDistTmp = kSTDist.GetSquared();
        if ( fSqrDistTmp < fSqrDist )
        {
            m_kClosestPoint0 = kSTDist.GetClosestPoint0();
            m_kClosestPoint1 = kSTDist.GetClosestPoint1();
            fSqrDist = fSqrDistTmp;
        }
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Triangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMV00 = m_rkTriangle0.V[0] + fT*rkVelocity0;
    Vector3<Real> kMV01 = m_rkTriangle0.V[1] + fT*rkVelocity0;
    Vector3<Real> kMV02 = m_rkTriangle0.V[2] + fT*rkVelocity0;
    Vector3<Real> kMV10 = m_rkTriangle1.V[0] + fT*rkVelocity1;
    Vector3<Real> kMV11 = m_rkTriangle1.V[1] + fT*rkVelocity1;
    Vector3<Real> kMV12 = m_rkTriangle1.V[2] + fT*rkVelocity1;
    Triangle3<Real> kMTriangle0(kMV00,kMV01,kMV02);
    Triangle3<Real> kMTriangle1(kMV10,kMV11,kMV12);
    return DistTriangle3Triangle3<Real>(kMTriangle0,kMTriangle1).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Triangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMV00 = m_rkTriangle0.V[0] + fT*rkVelocity0;
    Vector3<Real> kMV01 = m_rkTriangle0.V[1] + fT*rkVelocity0;
    Vector3<Real> kMV02 = m_rkTriangle0.V[2] + fT*rkVelocity0;
    Vector3<Real> kMV10 = m_rkTriangle1.V[0] + fT*rkVelocity1;
    Vector3<Real> kMV11 = m_rkTriangle1.V[1] + fT*rkVelocity1;
    Vector3<Real> kMV12 = m_rkTriangle1.V[2] + fT*rkVelocity1;
    Triangle3<Real> kMTriangle0(kMV00,kMV01,kMV02);
    Triangle3<Real> kMTriangle1(kMV10,kMV11,kMV12);
    return DistTriangle3Triangle3<Real>(kMTriangle0,kMTriangle1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistTriangle3Triangle3<float>;

// double
template WM3_ITEM
class DistTriangle3Triangle3<double>;
//----------------------------------------------------------------------------
}
