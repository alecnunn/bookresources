// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSegment2Circle2.h"
#include "Wm3IntrLine2Circle2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment2Circle2<Real>::IntrSegment2Circle2 (
    const Segment2<Real>& rkSegment,
    const Circle2<Real>& rkCircle)
    :
    m_rkSegment(rkSegment),
    m_rkCircle(rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment2<Real>& IntrSegment2Circle2<Real>::GetSegment () const
{
    return m_rkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrSegment2Circle2<Real>::GetCircle () const
{
    return m_rkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment2Circle2<Real>::Find ()
{
    Real afT[2];
    bool bIntersects = IntrLine2Circle2<Real>::Find(m_rkSegment.Origin,
        m_rkSegment.Direction,m_rkCircle.Center,m_rkCircle.Radius,m_iQuantity,
        afT);

    if ( bIntersects )
    {
        // reduce root count if line-circle intersections are not on segment
        if ( m_iQuantity == 1 )
        {
            if ( Math<Real>::FAbs(afT[0]) > m_rkSegment.Extent )
                m_iQuantity = 0;
        }
        else
        {
            if ( afT[1] < -m_rkSegment.Extent
            ||   afT[0] >  m_rkSegment.Extent )
            {
                m_iQuantity = 0;
            }
            else
            {
                if ( afT[1] <= m_rkSegment.Extent )
                {
                    if ( afT[0] < -m_rkSegment.Extent )
                    {
                        m_iQuantity = 1;
                        afT[0] = afT[1];
                    }
                }
                else
                {
                    m_iQuantity = ( afT[0] >= -m_rkSegment.Extent ? 1 : 0 );
                }
            }
        }

        for (int i = 0; i < m_iQuantity; i++)
            m_akPoint[i] = m_rkSegment.Origin + afT[i]*m_rkSegment.Direction;
    }

    m_iIntersectionType = ( m_iQuantity > 0 ? IT_POINT : IT_EMPTY );
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment2Circle2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrSegment2Circle2<Real>::GetPoint (int i)
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
class IntrSegment2Circle2<float>;

// double
template WM3_ITEM
class IntrSegment2Circle2<double>;
//----------------------------------------------------------------------------
}
