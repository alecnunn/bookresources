// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrPlane3Lozenge3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Lozenge3<Real>::IntrPlane3Lozenge3 (const Plane3<Real>& rkPlane,
    const Lozenge3<Real>& rkLozenge)
    :
    m_rkPlane(rkPlane),
    m_rkLozenge(rkLozenge)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Lozenge3<Real>::GetPlane () const
{
    return m_rkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrPlane3Lozenge3<Real>::GetLozenge () const
{
    return m_rkLozenge;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Lozenge3<Real>::Test ()
{
    Real fSDistMM = m_rkPlane.DistanceTo(m_rkLozenge.Rectangle.GetMMCorner());
    Real fSDistPM = m_rkPlane.DistanceTo(m_rkLozenge.Rectangle.GetPMCorner());
    if ( fSDistMM*fSDistPM <= (Real)0.0 )
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    Real fSDistMP = m_rkPlane.DistanceTo(m_rkLozenge.Rectangle.GetMPCorner());
    if ( fSDistMM*fSDistMP <= (Real)0.0 )
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    Real fSDistPP = m_rkPlane.DistanceTo(m_rkLozenge.Rectangle.GetPPCorner());
    if ( fSDistPM*fSDistPP <= (Real)0.0 )
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    // The lozenge rectangle corners are all on one side of the plane.
    // The spheres centered at the corners, with radius that of the lozenge,
    // might intersect the plane.
    return Math<Real>::FAbs(fSDistMM) <= m_rkLozenge.Radius
        || Math<Real>::FAbs(fSDistPM) <= m_rkLozenge.Radius
        || Math<Real>::FAbs(fSDistMP) <= m_rkLozenge.Radius
        || Math<Real>::FAbs(fSDistPP) <= m_rkLozenge.Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Lozenge3<Real>::LozengeIsCulled () const
{
    Real fSDistMM = m_rkPlane.DistanceTo(m_rkLozenge.Rectangle.GetMMCorner());
    if ( fSDistMM < (Real)0.0 )
    {
        Real fSDistPM = m_rkPlane.DistanceTo(
            m_rkLozenge.Rectangle.GetPMCorner());
        if ( fSDistPM < (Real)0.0 )
        {
            Real fSDistMP = m_rkPlane.DistanceTo(
                m_rkLozenge.Rectangle.GetMPCorner());
            if ( fSDistMP < (Real)0.0 )
            {
                Real fSDistPP = m_rkPlane.DistanceTo(
                    m_rkLozenge.Rectangle.GetPPCorner());
                if ( fSDistPP < (Real)0.0 )
                {
                    // all four lozenge corners on negative side of plane
                    if ( fSDistMM <= fSDistPM )
                    {
                        if ( fSDistMM <= fSDistMP )
                            return fSDistMM <= -m_rkLozenge.Radius;
                        else
                            return fSDistMP <= -m_rkLozenge.Radius;
                    }
                    else
                    {
                        if ( fSDistPM <= fSDistPP )
                            return fSDistPM <= -m_rkLozenge.Radius;
                        else
                            return fSDistPP <= -m_rkLozenge.Radius;
                    }
                }
            }
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
class IntrPlane3Lozenge3<float>;

// double
template WM3_ITEM
class IntrPlane3Lozenge3<double>;
//----------------------------------------------------------------------------
}
