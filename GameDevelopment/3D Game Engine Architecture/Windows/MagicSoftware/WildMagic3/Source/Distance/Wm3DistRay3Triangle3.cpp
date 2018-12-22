// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistRay3Triangle3.h"
#include "Wm3DistRay3Segment3.h"
#include "Wm3DistVector3Triangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistRay3Triangle3<Real>::DistRay3Triangle3 (const Ray3<Real>& rkRay,
    const Triangle3<Real>& rkTriangle)
    :
    m_rkRay(rkRay),
    m_rkTriangle(rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& DistRay3Triangle3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& DistRay3Triangle3<Real>::GetTriangle () const
{
    return m_rkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Triangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Triangle3<Real>::GetSquared ()
{
    // compare ray to all three edges of the triangle
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        Segment3<Real> kEdge;
        kEdge.Origin = ((Real)0.5)*(m_rkTriangle.V[i0] + m_rkTriangle.V[i1]);
        kEdge.Direction = m_rkTriangle.V[i1] - m_rkTriangle.V[i0];
        kEdge.Extent = ((Real)0.5)*kEdge.Direction.Normalize();
        DistRay3Segment3<Real> kRSDist(m_rkRay,kEdge);
        fSqrDistTmp = kRSDist.GetSquared();
        if ( fSqrDistTmp < fSqrDist )
        {
            m_kClosestPoint0 = kRSDist.GetClosestPoint0();
            m_kClosestPoint1 = kRSDist.GetClosestPoint1();
            fSqrDist = fSqrDistTmp;
        }
    }

    // compare ray origin to triangle interior
    DistVector3Triangle3<Real> kVTDist(m_rkRay.Origin,m_rkTriangle);
    fSqrDistTmp = kVTDist.GetSquared();
    if ( fSqrDistTmp < fSqrDist )
    {
        m_kClosestPoint0 = kVTDist.GetClosestPoint0();
        m_kClosestPoint1 = kVTDist.GetClosestPoint1();
        fSqrDist = fSqrDistTmp;
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Triangle3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkRay.Origin + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_rkTriangle.V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_rkTriangle.V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_rkTriangle.V[2] + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin,m_rkRay.Direction);
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    return DistRay3Triangle3<Real>(kMRay,kMTriangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Triangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkRay.Origin + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_rkTriangle.V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_rkTriangle.V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_rkTriangle.V[2] + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin,m_rkRay.Direction);
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    return DistRay3Triangle3<Real>(kMRay,kMTriangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistRay3Triangle3<float>;

// double
template WM3_ITEM
class DistRay3Triangle3<double>;
//----------------------------------------------------------------------------
}
