// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSegment3Lozenge3.h"
#include "Wm3DistSegment3Rectangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment3Lozenge3<Real>::IntrSegment3Lozenge3 (
    const Segment3<Real>& rkSegment, const Lozenge3<Real>& rkLozenge)
    :
    m_rkSegment(rkSegment),
    m_rkLozenge(rkLozenge)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& IntrSegment3Lozenge3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrSegment3Lozenge3<Real>::GetLozenge () const
{
    return m_rkLozenge;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Lozenge3<Real>::Test ()
{
    Real fDist = DistSegment3Rectangle3<Real>(m_rkSegment,
        m_rkLozenge.Rectangle).Get();
    return fDist <= m_rkLozenge.Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrSegment3Lozenge3<float>;

// double
template WM3_ITEM
class IntrSegment3Lozenge3<double>;
//----------------------------------------------------------------------------
}
