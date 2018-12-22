// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistSegment3Triangle3.h"
#include "Wm3DistSegment3Segment3.h"
#include "Wm3DistVector3Triangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistSegment3Triangle3<Real>::DistSegment3Triangle3 (
    const Segment3<Real>& rkSegment, const Triangle3<Real>& rkTriangle)
    :
    m_rkSegment(rkSegment),
    m_rkTriangle(rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistSegment3Triangle3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& DistSegment3Triangle3<Real>::GetTriangle () const
{
    return m_rkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::GetSquared ()
{
    // compare segment to all three edges of the triangle
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        Segment3<Real> kEdge;
        kEdge.Origin = ((Real)0.5)*(m_rkTriangle.V[i0] + m_rkTriangle.V[i1]);
        kEdge.Direction = m_rkTriangle.V[i1] - m_rkTriangle.V[i0];
        kEdge.Extent = ((Real)0.5)*kEdge.Direction.Normalize();
        DistSegment3Segment3<Real> kSSDist(m_rkSegment,kEdge);
        fSqrDistTmp = kSSDist.GetSquared();
        if ( fSqrDistTmp < fSqrDist )
        {
            m_kClosestPoint0 = kSSDist.GetClosestPoint0();
            m_kClosestPoint1 = kSSDist.GetClosestPoint1();
            fSqrDist = fSqrDistTmp;
        }
    }

    // compare segment end points to triangle interior
    Vector3<Real> kPosEnd = m_rkSegment.GetPosEnd();
    DistVector3Triangle3<Real> kPosDist(kPosEnd,m_rkTriangle);
    fSqrDistTmp = kPosDist.GetSquared();
    if ( fSqrDistTmp < fSqrDist )
    {
        m_kClosestPoint0 = kPosDist.GetClosestPoint0();
        m_kClosestPoint1 = kPosDist.GetClosestPoint1();
        fSqrDist = fSqrDistTmp;
    }

    Vector3<Real> kNegEnd = m_rkSegment.GetNegEnd();
    DistVector3Triangle3<Real> kNegDist(kNegEnd,m_rkTriangle);
    fSqrDistTmp = kNegDist.GetSquared();
    if ( fSqrDistTmp < fSqrDist )
    {
        m_kClosestPoint0 = kNegDist.GetClosestPoint0();
        m_kClosestPoint1 = kNegDist.GetClosestPoint1();
        fSqrDist = fSqrDistTmp;
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkSegment.Origin + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_rkTriangle.V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_rkTriangle.V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_rkTriangle.V[2] + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_rkSegment.Direction,
        m_rkSegment.Extent);
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    return DistSegment3Triangle3<Real>(kMSegment,kMTriangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkSegment.Origin + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_rkTriangle.V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_rkTriangle.V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_rkTriangle.V[2] + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_rkSegment.Direction,
        m_rkSegment.Extent);
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    return DistSegment3Triangle3<Real>(kMSegment,kMTriangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistSegment3Triangle3<float>;

// double
template WM3_ITEM
class DistSegment3Triangle3<double>;
//----------------------------------------------------------------------------
}
