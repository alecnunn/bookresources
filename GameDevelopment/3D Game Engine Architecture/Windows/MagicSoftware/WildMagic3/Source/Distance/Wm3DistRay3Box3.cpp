// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistRay3Box3.h"
#include "Wm3DistVector3Box3.h"
#include "Wm3DistLine3Box3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistRay3Box3<Real>::DistRay3Box3 (const Ray3<Real>& rkRay,
    const Box3<Real>& rkBox)
    :
    m_rkRay(rkRay),
    m_rkBox(rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& DistRay3Box3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& DistRay3Box3<Real>::GetBox () const
{
    return m_rkBox;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Box3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Box3<Real>::GetSquared ()
{
    Line3<Real> kLine(m_rkRay.Origin,m_rkRay.Direction);
    DistLine3Box3<Real> kLBDist(kLine,m_rkBox);

    Real fSqrDistance;
    if ( kLBDist.GetLineParameter() >= (Real)0.0 )
    {
        fSqrDistance = kLBDist.GetSquared();
        m_kClosestPoint0 = kLBDist.GetClosestPoint0();
        m_kClosestPoint1 = kLBDist.GetClosestPoint1();
    }
    else
    {
        DistVector3Box3<Real> kVBDist(m_rkRay.Origin,m_rkBox);
        fSqrDistance = kVBDist.GetSquared();
        m_kClosestPoint0 = kVBDist.GetClosestPoint0();
        m_kClosestPoint1 = kVBDist.GetClosestPoint1();
    }
    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Box3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkRay.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkBox.Center + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin,m_rkRay.Direction);
    Box3<Real> kMBox(kMCenter,m_rkBox.Axis,m_rkBox.Extent);
    return DistRay3Box3<Real>(kMRay,kMBox).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay3Box3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkRay.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkBox.Center + fT*rkVelocity1;
    Ray3<Real> kMRay(kMOrigin,m_rkRay.Direction);
    Box3<Real> kMBox(kMCenter,m_rkBox.Axis,m_rkBox.Extent);
    return DistRay3Box3<Real>(kMRay,kMBox).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistRay3Box3<float>;

// double
template WM3_ITEM
class DistRay3Box3<double>;
//----------------------------------------------------------------------------
}
