// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistVector3Circle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Circle3<Real>::DistVector3Circle3 (
    const Vector3<Real>& rkVector, const Circle3<Real>& rkCircle)
    :
    m_rkVector(rkVector),
    m_rkCircle(rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Circle3<Real>::GetVector () const
{
    return m_rkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle3<Real>& DistVector3Circle3<Real>::GetCircle () const
{
    return m_rkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::GetSquared ()
{
    // signed distance from point to plane of circle
    Vector3<Real> kDiff0 = m_rkVector - m_rkCircle.Center;
    Real fDist = kDiff0.Dot(m_rkCircle.N);

    // projection of P-C onto plane is Q-C = P-C - (fDist)*N
    Vector3<Real> kDiff1 = kDiff0 - fDist*m_rkCircle.N;
    Real fSqrLen = kDiff1.SquaredLength();
    Real fSqrDistance;

    if ( fSqrLen >= Math<Real>::ZERO_TOLERANCE )
    {
        m_kClosestPoint1 = m_rkCircle.Center + (m_rkCircle.Radius/
            Math<Real>::Sqrt(fSqrLen))*kDiff1;
        Vector3<Real> kDiff2 = m_rkVector - m_kClosestPoint1;
        fSqrDistance = kDiff2.SquaredLength();
    }
    else
    {
        m_kClosestPoint1 = Vector3<Real>(Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,Math<Real>::MAX_REAL);
        fSqrDistance = m_rkCircle.Radius*m_rkCircle.Radius + fDist*fDist;
    }

    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkCircle.Center + fT*rkVelocity1;
    Circle3<Real> kMCircle(kMCenter,m_rkCircle.U,m_rkCircle.V,
        m_rkCircle.N,m_rkCircle.Radius);
    return DistVector3Circle3<Real>(kMVector,kMCircle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkCircle.Center + fT*rkVelocity1;
    Circle3<Real> kMCircle(kMCenter,m_rkCircle.U,m_rkCircle.V,
        m_rkCircle.N,m_rkCircle.Radius);
    return DistVector3Circle3<Real>(kMVector,kMCircle).Get();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistVector3Circle3<float>;

// double
template WM3_ITEM
class DistVector3Circle3<double>;
//----------------------------------------------------------------------------
}
