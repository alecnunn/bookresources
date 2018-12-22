// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrCapsule3Capsule3.h"
#include "Wm3DistSegment3Segment3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrCapsule3Capsule3<Real>::IntrCapsule3Capsule3 (
    const Capsule3<Real>& rkCapsule0, const Capsule3<Real>& rkCapsule1)
    :
    m_rkCapsule0(rkCapsule0),
    m_rkCapsule1(rkCapsule1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrCapsule3Capsule3<Real>::GetCapsule0 () const
{
    return m_rkCapsule0;
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrCapsule3Capsule3<Real>::GetCapsule1 () const
{
    return m_rkCapsule1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrCapsule3Capsule3<Real>::Test ()
{
    Real fDistance = DistSegment3Segment3<Real>(m_rkCapsule0.Segment,
        m_rkCapsule1.Segment).Get();
    Real fRSum = m_rkCapsule0.Radius + m_rkCapsule1.Radius;
    return fDistance <= fRSum;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrCapsule3Capsule3<float>;

// double
template WM3_ITEM
class IntrCapsule3Capsule3<double>;
//----------------------------------------------------------------------------
}
