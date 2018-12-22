// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrRay2Circle2.h"
#include "Wm3IntrLine2Circle2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay2Circle2<Real>::IntrRay2Circle2 (const Ray2<Real>& rkRay,
    const Circle2<Real>& rkCircle)
    :
    m_rkRay(rkRay),
    m_rkCircle(rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray2<Real>& IntrRay2Circle2<Real>::GetRay () const
{
    return m_rkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrRay2Circle2<Real>::GetCircle () const
{
    return m_rkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay2Circle2<Real>::Find ()
{
    Real afT[2];
    bool bIntersects = IntrLine2Circle2<Real>::Find(m_rkRay.Origin,
        m_rkRay.Direction,m_rkCircle.Center,m_rkCircle.Radius,m_iQuantity,
        afT);

    if ( bIntersects )
    {
        // reduce root count if line-circle intersections are not on ray
        if ( m_iQuantity == 1 )
        {
            if ( afT[0] < (Real)0.0 )
                m_iQuantity = 0;
        }
        else
        {
            if ( afT[1] < (Real)0.0 )
            {
                m_iQuantity = 0;
            }
            else if ( afT[0] < (Real)0.0 )
            {
                m_iQuantity = 1;
                afT[0] = afT[1];
            }
        }

        for (int i = 0; i < m_iQuantity; i++)
            m_akPoint[i] = m_rkRay.Origin + afT[i]*m_rkRay.Direction;
    }

    m_iIntersectionType = ( m_iQuantity > 0 ? IT_POINT : IT_EMPTY );
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay2Circle2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrRay2Circle2<Real>::GetPoint (int i)
    const
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
class IntrRay2Circle2<float>;

// double
template WM3_ITEM
class IntrRay2Circle2<double>;
//----------------------------------------------------------------------------
}
