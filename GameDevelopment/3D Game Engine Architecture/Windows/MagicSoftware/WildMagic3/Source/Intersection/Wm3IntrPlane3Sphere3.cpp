// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrPlane3Sphere3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Sphere3<Real>::IntrPlane3Sphere3 (const Plane3<Real>& rkPlane,
    const Sphere3<Real>& rkSphere)
    :
    m_rkPlane(rkPlane),
    m_rkSphere(rkSphere)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Sphere3<Real>::GetPlane () const
{
    return m_rkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrPlane3Sphere3<Real>::GetSphere () const
{
    return m_rkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Sphere3<Real>::Test ()
{
    Real fSignedDistance = m_rkPlane.DistanceTo(m_rkSphere.Center);
    return Math<Real>::FAbs(fSignedDistance) <= m_rkSphere.Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Sphere3<Real>::SphereIsCulled () const
{
    Real fSignedDistance = m_rkPlane.DistanceTo(m_rkSphere.Center);
    return fSignedDistance <= -m_rkSphere.Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrPlane3Sphere3<float>;

// double
template WM3_ITEM
class IntrPlane3Sphere3<double>;
//----------------------------------------------------------------------------
}
