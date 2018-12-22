// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrRay3Sphere3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Sphere3<Real>::IntrRay3Sphere3 (const Ray3<Real>& rkRay,
    const Sphere3<Real>& rkSphere)
    :
    m_rkRay(rkRay),
    m_rkSphere(rkSphere)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Sphere3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrRay3Sphere3<Real>::GetSphere () const
{
    return m_rkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Sphere3<Real>::Test ()
{
    Vector3<Real> kDiff = m_rkRay.Origin - m_rkSphere.Center;
    Real fA0 = kDiff.Dot(kDiff) - m_rkSphere.Radius*m_rkSphere.Radius;
    if ( fA0 <= (Real)0.0 )
    {
        // P is inside the sphere
        return true;
    }
    // else: P is outside the sphere

    Real fA1 = m_rkRay.Direction.Dot(kDiff);
    if ( fA1 >= (Real)0.0 )
        return false;

    // quadratic has a real root if discriminant is nonnegative
    return fA1*fA1 >= fA0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Sphere3<Real>::Find ()
{
    Vector3<Real> kDiff = m_rkRay.Origin - m_rkSphere.Center;
    Real fA0 = kDiff.Dot(kDiff) - m_rkSphere.Radius*m_rkSphere.Radius;
    Real fA1, fDiscr, fRoot;
    if ( fA0 <= (Real)0.0 )
    {
        // P is inside the sphere
        fA1 = m_rkRay.Direction.Dot(kDiff);
        fDiscr = fA1*fA1 - fA0;
        fRoot = Math<Real>::Sqrt(fDiscr);
        m_afRayT[0] = -fA1 + fRoot;
        m_akPoint[0] = m_rkRay.Origin + m_afRayT[0]*m_rkRay.Direction;
        return true;
    }
    // else: P is outside the sphere

    fA1 = m_rkRay.Direction.Dot(kDiff);
    if ( fA1 >= (Real)0.0 )
    {
        m_iQuantity = 0;
        return false;
    }

    fDiscr = fA1*fA1 - fA0;
    if ( fDiscr < (Real)0.0 )
    {
        m_iQuantity = 0;
    }
    else if ( fDiscr >= Math<Real>::ZERO_TOLERANCE )
    {
        fRoot = Math<Real>::Sqrt(fDiscr);
        m_afRayT[0] = -fA1 - fRoot;
        m_afRayT[1] = -fA1 + fRoot;
        m_akPoint[0] = m_rkRay.Origin + m_afRayT[0]*m_rkRay.Direction;
        m_akPoint[1] = m_rkRay.Origin + m_afRayT[1]*m_rkRay.Direction;
        m_iQuantity = 2;
    }
    else
    {
        m_afRayT[0] = -fA1;
        m_akPoint[0] = m_rkRay.Origin + m_afRayT[0]*m_rkRay.Direction;
        m_iQuantity = 1;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay3Sphere3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrRay3Sphere3<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrRay3Sphere3<Real>::GetRayT (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_afRayT[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrRay3Sphere3<float>;

// double
template WM3_ITEM
class IntrRay3Sphere3<double>;
//----------------------------------------------------------------------------
}
