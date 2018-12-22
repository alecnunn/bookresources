// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSegment2Arc2.h"
#include "Wm3IntrLine2Circle2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment2Arc2<Real>::IntrSegment2Arc2 (
    const Segment2<Real>& rkSegment,
    const Arc2<Real>& rkArc)
    :
    m_rkSegment(rkSegment),
    m_rkArc(rkArc)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment2<Real>& IntrSegment2Arc2<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrSegment2Arc2<Real>::GetArc () const
{
    return m_rkArc;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment2Arc2<Real>::Find ()
{
    Real afT[2];
    int iQuantity;
    bool bIntersects = IntrLine2Circle2<Real>::Find(m_rkSegment.Origin,
        m_rkSegment.Direction,m_rkArc.Center,m_rkArc.Radius,iQuantity,
        afT);

    m_iQuantity = 0;
    if ( bIntersects )
    {
        // reduce root count if line-circle intersections are not on segment
        if ( iQuantity == 1 )
        {
            if ( Math<Real>::FAbs(afT[0]) > m_rkSegment.Extent )
                iQuantity = 0;
        }
        else
        {
            if ( afT[1] < -m_rkSegment.Extent
            ||   afT[0] >  m_rkSegment.Extent )
            {
                iQuantity = 0;
            }
            else
            {
                if ( afT[1] <= m_rkSegment.Extent )
                {
                    if ( afT[0] < -m_rkSegment.Extent )
                    {
                        iQuantity = 1;
                        afT[0] = afT[1];
                    }
                }
                else
                {
                    iQuantity = ( afT[0] >= -m_rkSegment.Extent ? 1 : 0 );
                }
            }
        }

        for (int i = 0; i < iQuantity; i++)
        {
            Vector2<Real> kPoint = m_rkSegment.Origin +
                m_rkSegment.Direction*afT[i];
            if ( m_rkArc.Contains(kPoint) )
                m_akPoint[m_iQuantity++] = kPoint;
        }
    }

    m_iIntersectionType = ( m_iQuantity > 0 ? IT_POINT : IT_EMPTY );
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment2Arc2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrSegment2Arc2<Real>::GetPoint (int i)
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
class IntrSegment2Arc2<float>;

// double
template WM3_ITEM
class IntrSegment2Arc2<double>;
//----------------------------------------------------------------------------
}
