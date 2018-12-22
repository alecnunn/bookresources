// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistRay3Segment3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistRay3Segment3<Real>::DistRay3Segment3 (const Ray3<Real>& rkRay,
    const Segment3<Real>& rkSegment)
    :
    m_rkRay(rkRay),
    m_rkSegment(rkSegment)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& DistRay3Segment3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistRay3Segment3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Segment3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Segment3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_rkRay.Origin- m_rkSegment.Origin;
    Real fA01 = -m_rkRay.Direction.Dot(m_rkSegment.Direction);
    Real fB0 = kDiff.Dot(m_rkRay.Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist, fTmp;

    if ( fDet >= Math<Real>::ZERO_TOLERANCE )
    {
        // ray and segment are not parallel
        fB1 = -kDiff.Dot(m_rkSegment.Direction);
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;

        if ( fS0 >= (Real)0.0 )
        {
            if ( fS1 >= (Real)0.0 )
            {
                if ( fS1 <= fDet )  // region 0
                {
                    // minimum at interior points of ray and segment
                    Real fInvDet = ((Real)1.0)/fDet;
                    fS0 *= fInvDet;
                    fS1 *= fInvDet;
                    fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
                        fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
                }
                else  // region 1
                {
                    fS1 = (Real)1.0;
                    if ( fB0 >= -fA01 )
                    {
                        fS0 = (Real)0.0;
                        fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                    }
                    else
                    {
                        fTmp = fA01 + fB0;
                        fS0 = -fTmp;
                        fSqrDist = fTmp*fS0+(Real)1.0+((Real)2.0)*fB1+fC;
                    }
                }
            }
            else  // region 5
            {
                fS1 = (Real)0.0;
                if ( fB0 >= (Real)0.0 )
                {
                    fS0 = (Real)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fS0 = -fB0;
                    fSqrDist = fB0*fS0+fC;
                }
            }
        }
        else
        {
            if ( fS1 <= (Real)0.0 )  // region 4
            {
                if ( fB0 < (Real)0.0 )
                {
                    fS0 = -fB0;
                    fS1 = (Real)0.0;
                    fSqrDist = fB0*fS0+fC;
                }
                else
                {
                    fS0 = (Real)0.0;
                    if ( fB1 >= (Real)0.0 )
                    {
                        fS1 = (Real)0.0;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= (Real)1.0 )
                    {
                        fS1 = (Real)1.0;
                        fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                    }
                    else
                    {
                        fS1 = -fB1;
                        fSqrDist = fB1*fS1+fC;
                    }
                }
            }
            else if ( fS1 <= fDet )  // region 3
            {
                fS0 = (Real)0.0;
                if ( fB1 >= (Real)0.0 )
                {
                    fS1 = (Real)0.0;
                    fSqrDist = fC;
                }
                else if ( -fB1 >= (Real)1.0 )
                {
                    fS1 = (Real)1.0;
                    fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                }
                else
                {
                    fS1 = -fB1;
                    fSqrDist = fB1*fS1+fC;
                }
            }
            else  // region 2
            {
                fTmp = fA01+fB0;
                if ( fTmp < (Real)0.0 )
                {
                    fS0 = -fTmp;
                    fS1 = (Real)1.0;
                    fSqrDist = fTmp*fS0+(Real)1.0+((Real)2.0)*fB1+fC;
                }
                else
                {
                    fS0 = (Real)0.0;
                    if ( fB1 >= (Real)0.0 )
                    {
                        fS1 = (Real)0.0;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= (Real)1.0 )
                    {
                        fS1 = (Real)1.0;
                        fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                    }
                    else
                    {
                        fS1 = -fB1;
                        fSqrDist = fB1*fS1+fC;
                    }
                }
            }
        }
    }
    else
    {
        // ray and segment are parallel
        if ( fA01 > (Real)0.0 )
        {
            // opposite direction vectors
            fS1 = (Real)0.0;
            if ( fB0 >= (Real)0.0 )
            {
                fS0 = (Real)0.0;
                fSqrDist = fC;
            }
            else
            {
                fS0 = -fB0;
                fSqrDist = fB0*fS0+fC;
            }
        }
        else
        {
            // same direction vectors
            fB1 = -kDiff.Dot(m_rkSegment.Direction);
            fS1 = (Real)1.0;
            fTmp = fA01+fB0;
            if ( fTmp >= (Real)0.0 )
            {
                fS0 = (Real)0.0;
                fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
            }
            else
            {
                fS0 = -fTmp;
                fSqrDist = fTmp*fS0+(Real)1.0+((Real)2.0)*fB1+fC;
            }
        }
    }

    m_kClosestPoint0 = m_rkRay.Origin + fS0*m_rkRay.Direction;
    m_kClosestPoint1 = m_rkSegment.Origin + fS1*m_rkSegment.Direction;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Segment3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkRay.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkSegment.Origin + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin0,m_rkRay.Direction);
    Segment3<Real> kMSegment(kMOrigin1,m_rkSegment.Direction,
        m_rkSegment.Extent);
    return DistRay3Segment3<Real>(kMRay,kMSegment).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Segment3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkRay.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkSegment.Origin + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin0,m_rkRay.Direction);
    Segment3<Real> kMSegment(kMOrigin1,m_rkSegment.Direction,
        m_rkSegment.Extent);
    return DistRay3Segment3<Real>(kMRay,kMSegment).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistRay3Segment3<float>;

// double
template WM3_ITEM
class DistRay3Segment3<double>;
//----------------------------------------------------------------------------
}
