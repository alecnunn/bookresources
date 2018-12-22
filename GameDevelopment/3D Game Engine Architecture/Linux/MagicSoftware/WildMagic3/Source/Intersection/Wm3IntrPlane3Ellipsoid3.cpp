// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrPlane3Ellipsoid3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Ellipsoid3<Real>::IntrPlane3Ellipsoid3 (const Plane3<Real>& rkPlane,
    const Ellipsoid3<Real>& rkEllipsoid)
    :
    m_rkPlane(rkPlane),
    m_rkEllipsoid(rkEllipsoid)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Ellipsoid3<Real>::GetPlane () const
{
    return m_rkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipsoid3<Real>& IntrPlane3Ellipsoid3<Real>::GetEllipsoid () const
{
    return m_rkEllipsoid;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Ellipsoid3<Real>::Test ()
{
    Matrix3<Real> kMInverse;
    m_rkEllipsoid.GetMInverse(kMInverse);
    Real fDiscr = kMInverse.QForm(m_rkPlane.Normal,m_rkPlane.Normal);
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    Real fSDist = m_rkPlane.DistanceTo(m_rkEllipsoid.Center);
    return Math<Real>::FAbs(fSDist) <= fRoot;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Ellipsoid3<Real>::EllipsoidIsCulled () const
{
    Matrix3<Real> kMInverse;
    m_rkEllipsoid.GetMInverse(kMInverse);
    Real fDiscr = kMInverse.QForm(m_rkPlane.Normal,m_rkPlane.Normal);
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    Real fSDist = m_rkPlane.DistanceTo(m_rkEllipsoid.Center);
    return fSDist <= -fRoot;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrPlane3Ellipsoid3<float>;

// double
template WM3_ITEM
class IntrPlane3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
