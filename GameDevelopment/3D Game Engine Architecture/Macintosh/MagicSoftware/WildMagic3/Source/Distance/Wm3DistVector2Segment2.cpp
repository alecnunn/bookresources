// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistVector2Segment2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistVector2Segment2<Real>::DistVector2Segment2 (const Vector2<Real>& rkVector,
    const Segment2<Real>& rkSegment)
    :
    m_rkVector(rkVector),
    m_rkSegment(rkSegment)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& DistVector2Segment2<Real>::GetVector () const
{
    return m_rkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment2<Real>& DistVector2Segment2<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Segment2<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Segment2<Real>::GetSquared ()
{
    Vector2<Real> kDiff = m_rkVector - m_rkSegment.Origin;
    Real fParam = m_rkSegment.Direction.Dot(kDiff);

    if ( -m_rkSegment.Extent < fParam )
    {
        if ( fParam < m_rkSegment.Extent )
        {
            m_kClosestPoint1 = m_rkSegment.Origin +
                fParam*m_rkSegment.Direction;
        }
        else
        {
            m_kClosestPoint1 = m_rkSegment.Origin +
                m_rkSegment.Extent*m_rkSegment.Direction;
        }
    }
    else
    {
        m_kClosestPoint1 = m_rkSegment.Origin -
            m_rkSegment.Extent*m_rkSegment.Direction;
    }

    m_kClosestPoint0 = m_rkVector;
    kDiff = m_kClosestPoint1 - m_kClosestPoint0;
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Segment2<Real>::Get (Real fT,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector2<Real> kMOrigin = m_rkSegment.Origin + fT*rkVelocity1;
    Segment2<Real> kMSegment(kMOrigin,m_rkSegment.Direction,
        m_rkSegment.Extent);
    return DistVector2Segment2<Real>(kMVector,kMSegment).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Segment2<Real>::GetSquared (Real fT,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector2<Real> kMOrigin = m_rkSegment.Origin + fT*rkVelocity1;
    Segment2<Real> kMSegment(kMOrigin,m_rkSegment.Direction,
        m_rkSegment.Extent);
    return DistVector2Segment2<Real>(kMVector,kMSegment).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistVector2Segment2<float>;

// double
template WM3_ITEM
class DistVector2Segment2<double>;
//----------------------------------------------------------------------------
}
