// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSegment3Triangle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment3Triangle3<Real>::IntrSegment3Triangle3 (
    const Segment3<Real>& rkSegment, const Triangle3<Real>& rkTriangle)
    :
    m_rkSegment(rkSegment),
    m_rkTriangle(rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& IntrSegment3Triangle3<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrSegment3Triangle3<Real>::GetTriangle () const
{
    return m_rkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Triangle3<Real>::Test ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_rkSegment.Origin - m_rkTriangle.V[0];
    Vector3<Real> kEdge1 = m_rkTriangle.V[1] - m_rkTriangle.V[0];
    Vector3<Real> kEdge2 = m_rkTriangle.V[2] - m_rkTriangle.V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = segment direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_rkSegment.Direction.Dot(kNormal);
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
        // Segment and triangle are parallel, call it a "no intersection"
        // even if the segment does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_rkSegment.Direction.Dot(kDiff.Cross(kEdge2));
    if ( fDdQxE2 >= (Real)0.0 )
    {
        Real fDdE1xQ = fSign*m_rkSegment.Direction.Dot(kEdge1.Cross(kDiff));
        if ( fDdE1xQ >= (Real)0.0 )
        {
            if ( fDdQxE2 + fDdE1xQ <= fDdN )
            {
                // line intersects triangle, check if segment does
                Real fQdN = -fSign*kDiff.Dot(kNormal);
                Real fExtDdN = m_rkSegment.Extent*fDdN;
                if ( -fExtDdN <= fQdN && fQdN <= fExtDdN )
                {
                    // segment intersects triangle
                    return true;
                }
                // else: |t| > extent, no intersection
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
bool IntrSegment3Triangle3<Real>::Find ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_rkSegment.Origin - m_rkTriangle.V[0];
    Vector3<Real> kEdge1 = m_rkTriangle.V[1] - m_rkTriangle.V[0];
    Vector3<Real> kEdge2 = m_rkTriangle.V[2] - m_rkTriangle.V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = segment direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_rkSegment.Direction.Dot(kNormal);
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
        // Segment and triangle are parallel, call it a "no intersection"
        // even if the segment does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_rkSegment.Direction.Dot(kDiff.Cross(kEdge2));
    if ( fDdQxE2 >= (Real)0.0 )
    {
        Real fDdE1xQ = fSign*m_rkSegment.Direction.Dot(kEdge1.Cross(kDiff));
        if ( fDdE1xQ >= (Real)0.0 )
        {
            if ( fDdQxE2 + fDdE1xQ <= fDdN )
            {
                // line intersects triangle, check if segment does
                Real fQdN = -fSign*kDiff.Dot(kNormal);
                Real fExtDdN = m_rkSegment.Extent*fDdN;
                if ( -fExtDdN <= fQdN && fQdN <= fExtDdN )
                {
                    // segment intersects triangle
                    Real fInv = ((Real)1.0)/fDdN;
                    m_fSegmentT = fQdN*fInv;
                    m_fTriB1 = fDdQxE2*fInv;
                    m_fTriB2 = fDdE1xQ*fInv;
                    m_fTriB0 = (Real)1.0 - m_fTriB1 - m_fTriB2;
                    return true;
                }
                // else: |t| > extent, no intersection
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
Real IntrSegment3Triangle3<Real>::GetSegmentT () const
{
    return m_fSegmentT;
}
//----------------------------------------------------------------------------
template <class Real>
inline Real IntrSegment3Triangle3<Real>::GetTriB0 () const
{
    return m_fTriB0;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSegment3Triangle3<Real>::GetTriB1 () const
{
    return m_fTriB1;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSegment3Triangle3<Real>::GetTriB2 () const
{
    return m_fTriB2;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrSegment3Triangle3<float>;

// double
template WM3_ITEM
class IntrSegment3Triangle3<double>;
//----------------------------------------------------------------------------
}
