// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistLine3Segment3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistLine3Segment3<Real>::DistLine3Segment3 (const Line3<Real>& rkLine,
    const Segment3<Real>& rkSegment)
    :
    m_rkLine(rkLine),
    m_rkSegment(rkSegment)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistLine3Segment3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistLine3Segment3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_rkLine.Origin - m_rkSegment.Origin;
    Real fA01 = -m_rkLine.Direction.Dot(m_rkSegment.Direction);
    Real fB0 = kDiff.Dot(m_rkLine.Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist;

    if ( fDet >= Math<Real>::ZERO_TOLERANCE )
    {
        fB1 = -kDiff.Dot(m_rkSegment.Direction);
        fS1 = fA01*fB0-fB1;

        if ( fS1 >= (Real)0.0 )
        {
            if ( fS1 <= fDet )
            {
                // two interior points are closest, one on line and one on
                // segment
                Real fInvDet = ((Real)1.0)/fDet;
                fS0 = (fA01*fB1-fB0)*fInvDet;
                fS1 *= fInvDet;
                fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
                    fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
            }
            else
            {
                // end point e1 of segment and interior point of line are
                // closest
                Real fTmp = fA01+fB0;
                fS0 = -fTmp;
                fS1 = (Real)1.0;
                fSqrDist = fTmp*fS0+(Real)1.0+((Real)2.0)*fB1+fC;
            }
        }
        else
        {
            // end point e0 of segment and interior point of line are closest
            fS0 = -fB0;
            fS1 = (Real)0.0;
            fSqrDist = fB0*fS0+fC;
        }
    }
    else
    {
        // lines are parallel, closest pair with one point at segment origin
        fS0 = -fB0;
        fS1 = (Real)0.0;
        fSqrDist = fB0*fS0+fC;
    }

    m_kClosestPoint0 = m_rkLine.Origin + fS0*m_rkLine.Direction;
    m_kClosestPoint1 = m_rkSegment.Origin + fS1*m_rkSegment.Direction;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkSegment.Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin0,m_rkLine.Direction);
    Segment3<Real> kMSegment(kMOrigin1,m_rkSegment.Direction,
        m_rkSegment.Extent);
    return DistLine3Segment3<Real>(kMLine,kMSegment).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkSegment.Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin0,m_rkLine.Direction);
    Segment3<Real> kMSegment(kMOrigin1,m_rkSegment.Direction,
        m_rkSegment.Extent);
    return DistLine3Segment3<Real>(kMLine,kMSegment).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistLine3Segment3<float>;

// double
template WM3_ITEM
class DistLine3Segment3<double>;
//----------------------------------------------------------------------------
}
