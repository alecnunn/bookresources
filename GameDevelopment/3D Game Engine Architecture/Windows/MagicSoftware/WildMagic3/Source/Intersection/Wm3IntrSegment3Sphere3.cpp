// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSegment3Sphere3.h"
#include "Wm3IntrSegment3Capsule3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment3Sphere3<Real>::IntrSegment3Sphere3 (
    const Segment3<Real>& rkSegment, const Sphere3<Real>& rkSphere)
    :
    m_rkSegment(rkSegment),
    m_rkSphere(rkSphere)
{
    m_iQuantity = 0;
    ZeroThreshold = Math<Real>::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& IntrSegment3Sphere3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrSegment3Sphere3<Real>::GetSphere () const
{
    return m_rkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Sphere3<Real>::Test ()
{
    Vector3<Real> kDiff = m_rkSegment.Origin - m_rkSphere.Center;
    Real fA0 = kDiff.Dot(kDiff) - m_rkSphere.Radius*m_rkSphere.Radius;
    Real fA1 = m_rkSegment.Direction.Dot(kDiff);
    Real fDiscr = fA1*fA1 - fA0;
    if ( fDiscr < (Real)0.0 )
        return false;

    Real fTmp0 = m_rkSegment.Extent*m_rkSegment.Extent + fA0;
    Real fTmp1 = ((Real)2.0)*fA1*m_rkSegment.Extent;
    Real fQM = fTmp0 - fTmp1;
    Real fQP = fTmp0 + fTmp1;
    if ( fQM*fQP <= (Real)0.0 )
        return true;

    return fQM > (Real)0.0 && Math<Real>::FAbs(fA1) < m_rkSegment.Extent;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Sphere3<Real>::Find ()
{
    Vector3<Real> kDiff = m_rkSegment.Origin - m_rkSphere.Center;
    Real fA0 = kDiff.Dot(kDiff) - m_rkSphere.Radius*m_rkSphere.Radius;
    Real fA1 = m_rkSegment.Direction.Dot(kDiff);
    Real fDiscr = fA1*fA1 - fA0;
    if ( fDiscr < (Real)0.0 )
    {
        m_iQuantity = 0;
        return false;
    }

    Real fTmp0 = m_rkSegment.Extent*m_rkSegment.Extent + fA0;
    Real fTmp1 = ((Real)2.0)*fA1*m_rkSegment.Extent;
    Real fQM = fTmp0 - fTmp1;
    Real fQP = fTmp0 + fTmp1;
    Real fRoot;
    if ( fQM*fQP <= (Real)0.0 )
    {
        fRoot = Math<Real>::Sqrt(fDiscr);
        m_afSegmentT[0] = ( fQM > (Real)0.0 ? -fA1 - fRoot : -fA1 + fRoot );
        m_akPoint[0] = m_rkSegment.Origin + m_afSegmentT[0] *
            m_rkSegment.Direction;
        m_iQuantity = 1;
        return true;
    }

    if ( fQM > (Real)0.0 && Math<Real>::FAbs(fA1) < m_rkSegment.Extent )
    {
        if ( fDiscr >= ZeroThreshold )
        {
            fRoot = Math<Real>::Sqrt(fDiscr);
            m_afSegmentT[0] = -fA1 - fRoot;
            m_afSegmentT[1] = -fA1 + fRoot;
            m_akPoint[0] = m_rkSegment.Origin + m_afSegmentT[0] *
                m_rkSegment.Direction;
            m_akPoint[1] = m_rkSegment.Origin + m_afSegmentT[1] *
                m_rkSegment.Direction;
            m_iQuantity = 2;
        }
        else
        {
            m_afSegmentT[0] = -fA1;
            m_akPoint[0] = m_rkSegment.Origin + m_afSegmentT[0] *
                m_rkSegment.Direction;
            m_iQuantity = 1;
        }
    }
    else
    {
        m_iQuantity = 0;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Sphere3<Real>::Test (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    // check if initially intersecting
    if ( Test() )
        return true;

    // Substract the segment velocity from the sphere velocity so that
    // the calculations are based in the coordinate system of the segment.
    // In this system, the line is of course stationary.  The sphere spans
    // a capsule.  The intersection test reduces to a static one between
    // a segment and a capsule.

    Capsule3<Real> kCapsule;
    kCapsule.Segment.Direction = rkVelocity1 - rkVelocity0;
    kCapsule.Segment.Origin = m_rkSphere.Center +
        ((Real)0.5)*kCapsule.Segment.Direction;
    kCapsule.Segment.Extent =
        ((Real)0.5)*kCapsule.Segment.Direction.Normalize();
    kCapsule.Radius = m_rkSphere.Radius;

    return IntrSegment3Capsule3<Real>(m_rkSegment,kCapsule).Test();
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Sphere3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    // check if initially intersecting
    if ( Find() )
    {
        m_fContactTime = (Real)0.0;
        m_iIntersectionType = IT_OTHER;
        return true;
    }

    // Substract the segment velocity from the sphere velocity so that
    // the calculations are based in the coordinate system of the segment.
    // In this system, the line is of course stationary.  The sphere spans
    // a capsule, but instead we will "grow" the segment by the sphere radius
    // and shrink the sphere to its center.  The problem is now to detect
    // the first time the moving center intersects the capsule formed by
    // the line segment and sphere radius.

    Capsule3<Real> kCapsule;
    kCapsule.Segment = m_rkSegment;
    kCapsule.Radius = m_rkSphere.Radius;

    Segment3<Real> kPath;
    kPath.Direction = rkVelocity1 - rkVelocity0;
    kPath.Origin = m_rkSphere.Center + ((Real)0.5)*kPath.Direction;
    kPath.Extent = ((Real)0.5)*kPath.Direction.Normalize();

    IntrSegment3Capsule3<Real> kIntr(kPath,kCapsule);
    if ( !kIntr.Find() || kIntr.GetParameter(0) > fTMax )
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    // We now know the sphere will intersect the segment.  This can happen
    // either at a segment end point or at a segment interior point.  We
    // need to determine which.
    m_fContactTime = kIntr.GetParameter(0);

    Vector3<Real> kMCenter = m_rkSphere.Center + 
        m_fContactTime*rkVelocity1;

    Vector3<Real> kMOrigin = m_rkSegment.Origin +
        m_fContactTime*rkVelocity0;

    Real fTmp = m_rkSegment.Direction.Dot(kMOrigin);
    Real fNegEnd = fTmp - m_rkSegment.Extent;
    Real fPosEnd = fTmp + m_rkSegment.Extent;
    Real fCenter = m_rkSegment.Direction.Dot(kMCenter);

    if ( fCenter < fNegEnd )
    {
        // intersection at segment end point P-e*D
        m_akPoint[0] = kMOrigin - m_rkSegment.Extent*m_rkSegment.Direction;
    }
    else if ( fCenter > fPosEnd )
    {
        // intersection at segment end point P+e*D
        m_akPoint[0] = kMOrigin + m_rkSegment.Extent*m_rkSegment.Direction;
    }
    else
    {
        // Intersection with interior point on edge.  Use the projection
        // along direction axis to find which point that is.
        m_akPoint[0] = kMOrigin + (fCenter-fNegEnd)/(fPosEnd-fNegEnd) *
            m_rkSegment.Direction;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment3Sphere3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSegment3Sphere3<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSegment3Sphere3<Real>::GetSegmentT (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_afSegmentT[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrSegment3Sphere3<float>;

// double
template WM3_ITEM
class IntrSegment3Sphere3<double>;
//----------------------------------------------------------------------------
}
