// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSphere3Frustum3.h"
#include "Wm3DistVector3Frustum3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSphere3Frustum3<Real>::IntrSphere3Frustum3 (const Sphere3<Real>& rkSphere,
    const Frustum3<Real>& rkFrustum)
    :
    m_rkSphere(rkSphere),
    m_rkFrustum(rkFrustum)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrSphere3Frustum3<Real>::GetSphere () const
{
    return m_rkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
const Frustum3<Real>& IntrSphere3Frustum3<Real>::GetFrustum () const
{
    return m_rkFrustum;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Frustum3<Real>::Test ()
{
    Real fDist = DistVector3Frustum3<Real>(m_rkSphere.Center,
        m_rkFrustum).Get();
    return fDist <= m_rkSphere.Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrSphere3Frustum3<float>;

// double
template WM3_ITEM
class IntrSphere3Frustum3<double>;
//----------------------------------------------------------------------------
}
