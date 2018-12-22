// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistRay3Ray3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistRay3Ray3<Real>::DistRay3Ray3 (const Ray3<Real>& rkRay0,
    const Ray3<Real>& rkRay1)
    :
    m_rkRay0(rkRay0),
    m_rkRay1(rkRay1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& DistRay3Ray3<Real>::GetRay0 () const
{
    return m_rkRay0;
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& DistRay3Ray3<Real>::GetRay1 () const
{
    return m_rkRay1;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Ray3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Ray3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_rkRay0.Origin - m_rkRay1.Origin;
    Real fA01 = -m_rkRay0.Direction.Dot(m_rkRay1.Direction);
    Real fB0 = kDiff.Dot(m_rkRay0.Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist;

    if ( fDet >= Math<Real>::ZERO_TOLERANCE )
    {
        // rays are not parallel
        fB1 = -kDiff.Dot(m_rkRay1.Direction);
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;

        if ( fS0 >= (Real)0.0 )
        {
            if ( fS1 >= (Real)0.0 )  // region 0 (interior)
            {
                // minimum at two interior points of rays
                Real fInvDet = ((Real)1.0)/fDet;
                fS0 *= fInvDet;
                fS1 *= fInvDet;
                fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
                    fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
            }
            else  // region 3 (side)
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
            if ( fS1 >= (Real)0.0 )  // region 1 (side)
            {
                fS0 = (Real)0.0;
                if ( fB1 >= (Real)0.0 )
                {
                    fS1 = (Real)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fS1 = -fB1;
                    fSqrDist = fB1*fS1+fC;
                }
            }
            else  // region 2 (corner)
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
        // rays are parallel
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
            if ( fB0 >= (Real)0.0 )
            {
                fB1 = -kDiff.Dot(m_rkRay1.Direction);
                fS0 = (Real)0.0;
                fS1 = -fB1;
                fSqrDist = fB1*fS1+fC;
            }
            else
            {
                fS0 = -fB0;
                fS1 = (Real)0.0;
                fSqrDist = fB0*fS0+fC;
            }
        }
    }

    m_kClosestPoint0 = m_rkRay0.Origin + fS0*m_rkRay0.Direction;
    m_kClosestPoint1 = m_rkRay1.Origin + fS1*m_rkRay1.Direction;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Ray3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkRay0.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkRay1.Origin + fT*rkVelocity1;
    Ray3<Real> kMRay0(kMOrigin0,m_rkRay0.Direction);
    Ray3<Real> kMRay1(kMOrigin1,m_rkRay1.Direction);
    return DistRay3Ray3<Real>(kMRay0,kMRay1).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Ray3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkRay0.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkRay1.Origin + fT*rkVelocity1;
    Ray3<Real> kMRay0(kMOrigin0,m_rkRay0.Direction);
    Ray3<Real> kMRay1(kMOrigin1,m_rkRay1.Direction);
    return DistRay3Ray3<Real>(kMRay0,kMRay1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistRay3Ray3<float>;

// double
template WM3_ITEM
class DistRay3Ray3<double>;
//----------------------------------------------------------------------------
}
