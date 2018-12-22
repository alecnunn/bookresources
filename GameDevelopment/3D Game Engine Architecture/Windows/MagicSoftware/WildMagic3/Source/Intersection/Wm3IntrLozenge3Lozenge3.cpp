// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLozenge3Lozenge3.h"
#include "Wm3DistRectangle3Rectangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLozenge3Lozenge3<Real>::IntrLozenge3Lozenge3 (
    const Lozenge3<Real>& rkLozenge0, const Lozenge3<Real>& rkLozenge1)
    :
    m_rkLozenge0(rkLozenge0),
    m_rkLozenge1(rkLozenge1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrLozenge3Lozenge3<Real>::GetLozenge0 () const
{
    return m_rkLozenge0;
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrLozenge3Lozenge3<Real>::GetLozenge1 () const
{
    return m_rkLozenge1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLozenge3Lozenge3<Real>::Test ()
{
    Real fDist = DistRectangle3Rectangle3<Real>(m_rkLozenge0.Rectangle,
        m_rkLozenge1.Rectangle).Get();
    Real fRSum = m_rkLozenge0.Radius + m_rkLozenge1.Radius;
    return fDist <= fRSum;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrLozenge3Lozenge3<float>;

// double
template WM3_ITEM
class IntrLozenge3Lozenge3<double>;
//----------------------------------------------------------------------------
}
