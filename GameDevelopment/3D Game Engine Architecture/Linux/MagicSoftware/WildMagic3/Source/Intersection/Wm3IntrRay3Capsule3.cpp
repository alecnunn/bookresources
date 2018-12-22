// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrRay3Capsule3.h"
#include "Wm3DistRay3Segment3.h"
#include "Wm3IntrLine3Capsule3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Capsule3<Real>::IntrRay3Capsule3 (const Ray3<Real>& rkRay,
    const Capsule3<Real>& rkCapsule)
    :
    m_rkRay(rkRay),
    m_rkCapsule(rkCapsule)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Capsule3<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrRay3Capsule3<Real>::GetCapsule () const
{
    return m_rkCapsule;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Capsule3<Real>::Test ()
{
    Real fDist = DistRay3Segment3<Real>(m_rkRay,m_rkCapsule.Segment).Get();
    return fDist <= m_rkCapsule.Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Capsule3<Real>::Find ()
{
    Real afT[2];
    int iQuantity = IntrLine3Capsule3<Real>::Find(m_rkRay.Origin,
        m_rkRay.Direction,m_rkCapsule,afT);

    m_iQuantity = 0;
    for (int i = 0; i < iQuantity; i++)
    {
        if ( afT[i] >= (Real)0.0 )
        {
            m_akPoint[m_iQuantity++] = m_rkRay.Origin +
                afT[i]*m_rkRay.Direction;
        }
    }

    if ( m_iQuantity == 2 )
        m_iIntersectionType = IT_SEGMENT;
    else if ( m_iQuantity == 1 )
        m_iIntersectionType = IT_POINT;
    else
        m_iIntersectionType = IT_EMPTY;

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay3Capsule3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrRay3Capsule3<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrRay3Capsule3<float>;

// double
template WM3_ITEM
class IntrRay3Capsule3<double>;
//----------------------------------------------------------------------------
}
