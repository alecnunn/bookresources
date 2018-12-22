// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrPlane3Capsule3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Capsule3<Real>::IntrPlane3Capsule3 (const Plane3<Real>& rkPlane,
    const Capsule3<Real>& rkCapsule)
    :
    m_rkPlane(rkPlane),
    m_rkCapsule(rkCapsule)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Capsule3<Real>::GetPlane () const
{
    return m_rkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrPlane3Capsule3<Real>::GetCapsule () const
{
    return m_rkCapsule;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Capsule3<Real>::Test ()
{
    Real fPDist = m_rkPlane.DistanceTo(m_rkCapsule.Segment.GetPosEnd());
    Real fNDist = m_rkPlane.DistanceTo(m_rkCapsule.Segment.GetNegEnd());
    if ( fPDist*fNDist <= (Real)0.0 )
    {
        // capsule segment end points on opposite sides of the plane
        return true;
    }

    // End points on same side of plane, but the end point spheres (with
    // radius of the capsule) might intersect the plane.
    return Math<Real>::FAbs(fPDist) <= m_rkCapsule.Radius
        || Math<Real>::FAbs(fNDist) <= m_rkCapsule.Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Capsule3<Real>::CapsuleIsCulled () const
{
    Real fPDist = m_rkPlane.DistanceTo(m_rkCapsule.Segment.GetPosEnd());
    if ( fPDist < (Real)0.0 )
    {
        Real fNDist = m_rkPlane.DistanceTo(m_rkCapsule.Segment.GetNegEnd());
        if ( fNDist < (Real)0.0 )
        {
            if ( fPDist <= fNDist )
                return fPDist <= -m_rkCapsule.Radius;
            else
                return fNDist <= -m_rkCapsule.Radius;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrPlane3Capsule3<float>;

// double
template WM3_ITEM
class IntrPlane3Capsule3<double>;
//----------------------------------------------------------------------------
}
