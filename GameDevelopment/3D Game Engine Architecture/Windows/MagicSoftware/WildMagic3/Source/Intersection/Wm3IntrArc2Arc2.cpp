// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrArc2Arc2.h"
#include "Wm3IntrCircle2Circle2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrArc2Arc2<Real>::IntrArc2Arc2 (const Arc2<Real>& rkArc0,
    const Arc2<Real>& rkArc1)
    :
    m_rkArc0(rkArc0),
    m_rkArc1(rkArc1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetArc0 () const
{
    return m_rkArc0;
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetArc1 () const
{
    return m_rkArc1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrArc2Arc2<Real>::Find ()
{
    m_iQuantity = 0;

    Circle2<Real> kCircle0(m_rkArc0.Center,m_rkArc0.Radius);
    Circle2<Real> kCircle1(m_rkArc1.Center,m_rkArc1.Radius);
    IntrCircle2Circle2<Real> kIntr(kCircle0,kCircle1);
    if ( !kIntr.Find() )
    {
        // arcs do not intersect
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    if ( kIntr.GetIntersectionType() == IT_OTHER )
    {
        // Arcs are cocircular.  Determine if they overlap.  Let arc0 be
        // <A0,A1> and arc1 be <B0,B1>, the points ordered counterclockwise
        // around the circle of the arc.
        if ( m_rkArc1.Contains(m_rkArc0.End0) )
        {
            if ( m_rkArc1.Contains(m_rkArc0.End1) )
            {
                // arc0 inside arc 1, <B0,A0,A1,B1>
                m_iIntersectionType = IT_OTHER;
                m_kIntersectionArc = m_rkArc0;
            }
            else
            {
                if ( m_rkArc0.End0 != m_rkArc1.End1 )
                {
                    // arc0 and arc1 overlap, <B0,A0,B1,A1>
                    m_iIntersectionType = IT_OTHER;
                    m_kIntersectionArc.Center = m_rkArc0.Center;
                    m_kIntersectionArc.Radius = m_rkArc0.Radius;
                    m_kIntersectionArc.End0 = m_rkArc0.End0;
                    m_kIntersectionArc.End1 = m_rkArc1.End1;
                }
                else
                {
                    // arc0 and arc1 share end point, <B0,A0,B1,A1>, A0 = B1
                    m_iIntersectionType = IT_POINT;
                    m_iQuantity = 1;
                    m_akPoint[0] = m_rkArc0.End0;
                }
            }
            return true;
        }

        if ( m_rkArc1.Contains(m_rkArc0.End1) )
        {
            if ( m_rkArc0.End1 != m_rkArc1.End0 )
            {
                // arc0 and arc1 overlap, <A0,B0,A1,B1>
                m_iIntersectionType = IT_OTHER;
                m_kIntersectionArc.Center = m_rkArc0.Center;
                m_kIntersectionArc.Radius = m_rkArc0.Radius;
                m_kIntersectionArc.End0 = m_rkArc1.End0;
                m_kIntersectionArc.End1 = m_rkArc0.End1;
            }
            else
            {
                // arc0 and arc1 share end point, <A0,B0,A1,B1>, B0 = A1
                m_iIntersectionType = IT_POINT;
                m_iQuantity = 1;
                m_akPoint[0] = m_rkArc1.End0;
            }
            return true;
        }

        if ( m_rkArc0.Contains(m_rkArc1.End0) )
        {
            // arc1 inside arc0, <A0,B0,B1,A1>
            m_iIntersectionType = IT_OTHER;
            m_kIntersectionArc = m_rkArc1;
            return true;
        }
        else
        {
            // arcs do not overlap, <A0,A1,B0,B1>
            m_iIntersectionType = IT_EMPTY;
            return false;
        }
    }

    // test if circle-circle intersection points are on the arcs
    for (int i = 0; i < kIntr.GetQuantity(); i++)
    {
        if ( m_rkArc0.Contains(kIntr.GetPoint(i))
        &&   m_rkArc1.Contains(kIntr.GetPoint(i)) )
        {
            m_akPoint[m_iQuantity++] = kIntr.GetPoint(i);
        }
    }

    m_iIntersectionType = ( m_iQuantity > 0 ? IT_POINT : IT_EMPTY );
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrArc2Arc2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrArc2Arc2<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetIntersectionArc () const
{
    return m_kIntersectionArc;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrArc2Arc2<float>;

// double
template WM3_ITEM
class IntrArc2Arc2<double>;
//----------------------------------------------------------------------------
}
