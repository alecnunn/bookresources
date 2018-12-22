// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLine3Lozenge3.h"
#include "Wm3DistLine3Rectangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Lozenge3<Real>::IntrLine3Lozenge3 (const Line3<Real>& rkLine,
    const Lozenge3<Real>& rkLozenge)
    :
    m_rkLine(rkLine),
    m_rkLozenge(rkLozenge)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Lozenge3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrLine3Lozenge3<Real>::GetLozenge () const
{
    return m_rkLozenge;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Lozenge3<Real>::Test ()
{
    Real fDist = DistLine3Rectangle3<Real>(m_rkLine,
        m_rkLozenge.Rectangle).Get();
    return fDist <= m_rkLozenge.Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrLine3Lozenge3<float>;

// double
template WM3_ITEM
class IntrLine3Lozenge3<double>;
//----------------------------------------------------------------------------
}
