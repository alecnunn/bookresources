// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistLine3Ray3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistLine3Ray3<Real>::DistLine3Ray3 (const Line3<Real>& rkLine,
    const Ray3<Real>& rkRay)
    :
    m_rkLine(rkLine),
    m_rkRay(rkRay)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistLine3Ray3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& DistLine3Ray3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Ray3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Ray3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_rkLine.Origin - m_rkRay.Origin;
    Real fA01 = -m_rkLine.Direction.Dot(m_rkRay.Direction);
    Real fB0 = kDiff.Dot(m_rkLine.Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist;

    if ( fDet >= Math<Real>::ZERO_TOLERANCE )
    {
        fB1 = -kDiff.Dot(m_rkRay.Direction);
        fS1 = fA01*fB0-fB1;

        if ( fS1 >= (Real)0.0 )
        {
            // two interior points are closest, one on line and one on ray
            Real fInvDet = ((Real)1.0)/fDet;
            fS0 = (fA01*fB1-fB0)*fInvDet;
            fS1 *= fInvDet;
            fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
                fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
        }
        else
        {
            // end point of ray and interior point of line are closest
            fS0 = -fB0;
            fS1 = (Real)0.0;
            fSqrDist = fB0*fS0+fC;
        }
    }
    else
    {
        // lines are parallel, closest pair with one point at ray origin
        fS0 = -fB0;
        fS1 = (Real)0.0;
        fSqrDist = fB0*fS0+fC;
    }

    m_kClosestPoint0 = m_rkLine.Origin + fS0*m_rkLine.Direction;
    m_kClosestPoint1 = m_rkRay.Origin + fS1*m_rkRay.Direction;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Ray3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkRay.Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin0,m_rkLine.Direction);
    Ray3<Real> kMRay(kMOrigin1,m_rkRay.Direction);
    return DistLine3Ray3<Real>(kMLine,kMRay).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Ray3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkRay.Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin0,m_rkLine.Direction);
    Ray3<Real> kMRay(kMOrigin1,m_rkRay.Direction);
    return DistLine3Ray3<Real>(kMLine,kMRay).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistLine3Ray3<float>;

// double
template WM3_ITEM
class DistLine3Ray3<double>;
//----------------------------------------------------------------------------
}
