// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistSegment3Rectangle3.h"
#include "Wm3DistSegment3Segment3.h"
#include "Wm3DistVector3Rectangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistSegment3Rectangle3<Real>::DistSegment3Rectangle3 (
    const Segment3<Real>& rkSegment, const Rectangle3<Real>& rkRectangle)
    :
    m_rkSegment(rkSegment),
    m_rkRectangle(rkRectangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistSegment3Rectangle3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistSegment3Rectangle3<Real>::GetRectangle () const
{
    return m_rkRectangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::GetSquared ()
{
    // compare line to all four edges of the rectangle
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    for (int i1 = 0; i1 < 2; i1++)
    {
        for (int i0 = -1; i0 <= 1; i0 += 2)
        {
            Segment3<Real> kEdge;
            kEdge.Origin = m_rkRectangle.Center +
                (i0*m_rkRectangle.Extent[1-i1]) *
                m_rkRectangle.Axis[1-i1];
            kEdge.Direction = m_rkRectangle.Axis[i1];
            kEdge.Extent = m_rkRectangle.Extent[i1];
            DistSegment3Segment3<Real> kSSDist(m_rkSegment,kEdge);
            fSqrDistTmp = kSSDist.GetSquared();
            if ( fSqrDistTmp < fSqrDist )
            {
                m_kClosestPoint0 = kSSDist.GetClosestPoint0();
                m_kClosestPoint1 = kSSDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }

    // compare segment end points to rectangle interior
    Vector3<Real> kPosEnd = m_rkSegment.GetPosEnd();
    DistVector3Rectangle3<Real> kPosDist(kPosEnd,m_rkRectangle);
    fSqrDistTmp = kPosDist.GetSquared();
    if ( fSqrDistTmp < fSqrDist )
    {
        m_kClosestPoint0 = kPosDist.GetClosestPoint0();
        m_kClosestPoint1 = kPosDist.GetClosestPoint1();
        fSqrDist = fSqrDistTmp;
    }

    Vector3<Real> kNegEnd = m_rkSegment.GetNegEnd();
    DistVector3Rectangle3<Real> kNegDist(kNegEnd,m_rkRectangle);
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
Real DistSegment3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkSegment.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_rkSegment.Direction,
        m_rkSegment.Extent);
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis,
        m_rkRectangle.Extent);
    return DistSegment3Rectangle3<Real>(kMSegment,kMRectangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkSegment.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_rkSegment.Direction,
        m_rkSegment.Extent);
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis,
        m_rkRectangle.Extent);
    return DistSegment3Rectangle3<Real>(kMSegment,kMRectangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistSegment3Rectangle3<float>;

// double
template WM3_ITEM
class DistSegment3Rectangle3<double>;
//----------------------------------------------------------------------------
}
