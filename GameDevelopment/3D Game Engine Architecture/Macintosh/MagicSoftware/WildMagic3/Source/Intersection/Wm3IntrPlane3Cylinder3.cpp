// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrPlane3Cylinder3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Cylinder3<Real>::IntrPlane3Cylinder3 (const Plane3<Real>& rkPlane,
    const Cylinder3<Real>& rkCylinder)
    :
    m_rkPlane(rkPlane),
    m_rkCylinder(rkCylinder)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Cylinder3<Real>::GetPlane () const
{
    return m_rkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Cylinder3<Real>& IntrPlane3Cylinder3<Real>::GetCylinder () const
{
    return m_rkCylinder;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Cylinder3<Real>::Test ()
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = m_rkPlane.DistanceTo(m_rkCylinder.Segment.Origin);
    Real fAbsNdW = Math<Real>::FAbs(m_rkPlane.Normal.Dot(
        m_rkCylinder.Segment.Direction));
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1.0
        - fAbsNdW*fAbsNdW));
    Real fTerm = m_rkCylinder.Radius*fRoot +
        ((Real)0.5)*m_rkCylinder.Height*fAbsNdW;

    // intersection occurs if and only if 0 is in the interval [min,max]
    return Math<Real>::FAbs(fSDist) <= fTerm;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Cylinder3<Real>::CylinderIsCulled () const
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = m_rkPlane.DistanceTo(m_rkCylinder.Segment.Origin);
    Real fAbsNdW = Math<Real>::FAbs(m_rkPlane.Normal.Dot(
        m_rkCylinder.Segment.Direction));
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1.0
        - fAbsNdW*fAbsNdW));
    Real fTerm = m_rkCylinder.Radius*fRoot +
        ((Real)0.5)*m_rkCylinder.Height*fAbsNdW;

    // culling occurs if and only if max <= 0
    return fSDist + fTerm <= (Real)0.0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrPlane3Cylinder3<float>;

// double
template WM3_ITEM
class IntrPlane3Cylinder3<double>;
//----------------------------------------------------------------------------
}
