// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrRay3Triangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Triangle3<Real>::IntrRay3Triangle3 (const Ray3<Real>& rkRay,
    const Triangle3<Real>& rkTriangle)
    :
    m_rkRay(rkRay),
    m_rkTriangle(rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Triangle3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrRay3Triangle3<Real>::GetTriangle () const
{
    return m_rkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Triangle3<Real>::Test ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_rkRay.Origin - m_rkTriangle.V[0];
    Vector3<Real> kEdge1 = m_rkTriangle.V[1] - m_rkTriangle.V[0];
    Vector3<Real> kEdge2 = m_rkTriangle.V[2] - m_rkTriangle.V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_rkRay.Direction.Dot(kNormal);
    Real fSign;
    if ( fDdN > Math<Real>::ZERO_TOLERANCE )
    {
        fSign = (Real)1.0;
    }
    else if ( fDdN < -Math<Real>::ZERO_TOLERANCE )
    {
        fSign = (Real)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Ray and triangle are parallel, call it a "no intersection"
        // even if the ray does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_rkRay.Direction.Dot(kDiff.Cross(kEdge2));
    if ( fDdQxE2 >= (Real)0.0 )
    {
        Real fDdE1xQ = fSign*m_rkRay.Direction.Dot(kEdge1.Cross(kDiff));
        if ( fDdE1xQ >= (Real)0.0 )
        {
            if ( fDdQxE2 + fDdE1xQ <= fDdN )
            {
                // line intersects triangle, check if ray does
                Real fQdN = -fSign*kDiff.Dot(kNormal);
                if ( fQdN >= (Real)0.0 )
                {
                    // ray intersects triangle
                    return true;
                }
                // else: t < 0, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Triangle3<Real>::Find ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_rkRay.Origin - m_rkTriangle.V[0];
    Vector3<Real> kEdge1 = m_rkTriangle.V[1] - m_rkTriangle.V[0];
    Vector3<Real> kEdge2 = m_rkTriangle.V[2] - m_rkTriangle.V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_rkRay.Direction.Dot(kNormal);
    Real fSign;
    if ( fDdN > Math<Real>::ZERO_TOLERANCE )
    {
        fSign = (Real)1.0;
    }
    else if ( fDdN < -Math<Real>::ZERO_TOLERANCE )
    {
        fSign = (Real)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Ray and triangle are parallel, call it a "no intersection"
        // even if the ray does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_rkRay.Direction.Dot(kDiff.Cross(kEdge2));
    if ( fDdQxE2 >= (Real)0.0 )
    {
        Real fDdE1xQ = fSign*m_rkRay.Direction.Dot(kEdge1.Cross(kDiff));
        if ( fDdE1xQ >= (Real)0.0 )
        {
            if ( fDdQxE2 + fDdE1xQ <= fDdN )
            {
                // line intersects triangle, check if ray does
                Real fQdN = -fSign*kDiff.Dot(kNormal);
                if ( fQdN >= (Real)0.0 )
                {
                    // ray intersects triangle
                    Real fInv = ((Real)1.0)/fDdN;
                    m_fRayT = fQdN*fInv;
                    m_fTriB1 = fDdQxE2*fInv;
                    m_fTriB2 = fDdE1xQ*fInv;
                    m_fTriB0 = (Real)1.0 - m_fTriB1 - m_fTriB2;
                    return true;
                }
                // else: t < 0, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrRay3Triangle3<Real>::GetRayT () const
{
    return m_fRayT;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrRay3Triangle3<Real>::GetTriB0 () const
{
    return m_fTriB0;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrRay3Triangle3<Real>::GetTriB1 () const
{
    return m_fTriB1;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrRay3Triangle3<Real>::GetTriB2 () const
{
    return m_fTriB2;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrRay3Triangle3<float>;

// double
template WM3_ITEM
class IntrRay3Triangle3<double>;
//----------------------------------------------------------------------------
}
