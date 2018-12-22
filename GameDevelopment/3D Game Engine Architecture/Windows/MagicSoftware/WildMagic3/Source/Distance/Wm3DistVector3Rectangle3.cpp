// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistVector3Rectangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Rectangle3<Real>::DistVector3Rectangle3 (
    const Vector3<Real>& rkVector, const Rectangle3<Real>& rkRectangle)
    :
    m_rkVector(rkVector),
    m_rkRectangle(rkRectangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Rectangle3<Real>::GetVector () const
{
    return m_rkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistVector3Rectangle3<Real>::GetRectangle ()
    const
{
    return m_rkRectangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_rkRectangle.Center - m_rkVector;
    Real fB0 = kDiff.Dot(m_rkRectangle.Axis[0]);
    Real fB1 = kDiff.Dot(m_rkRectangle.Axis[1]);
    Real fS0 = -fB0, fS1 = -fB1;
    Real fSqrDistance = kDiff.SquaredLength();

    if ( fS0 < -m_rkRectangle.Extent[0] )
    {
        fS0 = -m_rkRectangle.Extent[0];
    }
    else if ( fS0 > m_rkRectangle.Extent[0] )
    {
        fS0 = m_rkRectangle.Extent[0];
    }
    fSqrDistance += fS0*(fS0 + ((Real)2.0)*fB0);

    if ( fS1 < -m_rkRectangle.Extent[1] )
    {
        fS1 = -m_rkRectangle.Extent[1];
    }
    else if ( fS1 > m_rkRectangle.Extent[1] )
    {
        fS1 = m_rkRectangle.Extent[1];
    }
    fSqrDistance += fS1*(fS1 + ((Real)2.0)*fB1);

    // account for numerical round-off error
    if ( fSqrDistance < (Real)0.0 )
        fSqrDistance = (Real)0.0;

    m_kClosestPoint0 = m_rkVector;
    m_kClosestPoint1 = m_rkRectangle.Center + fS0*m_rkRectangle.Axis[0] +
        fS1*m_rkRectangle.Axis[1];

    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis[0],
        m_rkRectangle.Axis[1],m_rkRectangle.Extent[0],
        m_rkRectangle.Extent[1]);
    return DistVector3Rectangle3<Real>(kMVector,kMRectangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis[0],
        m_rkRectangle.Axis[1],m_rkRectangle.Extent[0],
        m_rkRectangle.Extent[1]);
    return DistVector3Rectangle3<Real>(kMVector,kMRectangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistVector3Rectangle3<float>;

// double
template WM3_ITEM
class DistVector3Rectangle3<double>;
//----------------------------------------------------------------------------
}
