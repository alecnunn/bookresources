// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrCircle2Circle2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrCircle2Circle2<Real>::IntrCircle2Circle2 (const Circle2<Real>& rkCircle0,
    const Circle2<Real>& rkCircle1)
    :
    m_rkCircle0(rkCircle0),
    m_rkCircle1(rkCircle1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrCircle2Circle2<Real>::GetCircle0 () const
{
    return m_rkCircle0;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrCircle2Circle2<Real>::GetCircle1 () const
{
    return m_rkCircle1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrCircle2Circle2<Real>::Find ()
{
    // The two circles are |X-C0| = R0 and |X-C1| = R1.  Define U = C1 - C0
    // and V = Perp(U) where Perp(x,y) = (y,-x).  Note that Dot(U,V) = 0 and
    // |V|^2 = |U|^2.  The intersection points X can be written in the form
    // X = C0+s*U+t*V and X = C1+(s-1)*U+t*V.  Squaring the circle equations
    // and substituting these formulas into them yields
    //   R0^2 = (s^2 + t^2)*|U|^2
    //   R1^2 = ((s-1)^2 + t^2)*|U|^2.
    // Subtracting and solving for s yields
    //   s = ((R0^2-R1^2)/|U|^2 + 1)/2
    // Then replace in the first equation and solve for t^2
    //   t^2 = (R0^2/|U|^2) - s^2.
    // In order for there to be solutions, the right-hand side must be
    // nonnegative.  Some algebra leads to the condition for existence of
    // solutions,
    //   (|U|^2 - (R0+R1)^2)*(|U|^2 - (R0-R1)^2) <= 0.
    // This reduces to
    //   |R0-R1| <= |U| <= |R0+R1|.
    // If |U| = |R0-R1|, then the circles are side-by-side and just tangent.
    // If |U| = |R0+R1|, then the circles are nested and just tangent.
    // If |R0-R1| < |U| < |R0+R1|, then the two circles to intersect in two
    // points.

    Vector2<Real> kU = m_rkCircle1.Center - m_rkCircle0.Center;
    Real fUSqrLen = kU.SquaredLength();
    Real fR0 = m_rkCircle0.Radius, fR1 = m_rkCircle1.Radius;
    Real fR0mR1 = fR0 - fR1;
    if ( fUSqrLen < Math<Real>::ZERO_TOLERANCE
    &&   Math<Real>::FAbs(fR0mR1) < Math<Real>::ZERO_TOLERANCE )
    {
        // circles are essentially the same
        m_iIntersectionType = IT_OTHER;
        m_iQuantity = 0;
        return true;
    }

    Real fR0mR1Sqr = fR0mR1*fR0mR1;
    if ( fUSqrLen < fR0mR1Sqr )
    {
        m_iIntersectionType = IT_EMPTY;
        m_iQuantity = 0;
        return false;
    }

    Real fR0pR1 = fR0 + fR1;
    Real fR0pR1Sqr = fR0pR1*fR0pR1;
    if ( fUSqrLen > fR0pR1Sqr )
    {
        m_iIntersectionType = IT_EMPTY;
        m_iQuantity = 0;
        return false;
    }

    if ( fUSqrLen < fR0pR1Sqr )
    {
        if ( fR0mR1Sqr < fUSqrLen )
        {
            Real fInvUSqrLen = ((Real)1.0)/fUSqrLen;
            Real fS = ((Real)0.5)*((fR0*fR0-fR1*fR1)*fInvUSqrLen+(Real)1.0);
            Vector2<Real> kTmp = m_rkCircle0.Center + fS*kU;

            Real fT = Math<Real>::Sqrt(fR0*fR0*fInvUSqrLen - fS*fS);
            Vector2<Real> kV(kU.Y(),-kU.X());
            m_iQuantity = 2;
            m_akPoint[0] = kTmp - fT*kV;
            m_akPoint[1] = kTmp + fT*kV;
        }
        else
        {
            // |U| = |R0-R1|, circles are tangent
            m_iQuantity = 1;
            m_akPoint[0] = m_rkCircle0.Center + (fR0/fR0mR1)*kU;
        }
    }
    else
    {
        // |U| = |R0+R1|, circles are tangent
        m_iQuantity = 1;
        m_akPoint[0] = m_rkCircle0.Center + (fR0/fR0pR1)*kU;
    }

    m_iIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrCircle2Circle2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrCircle2Circle2<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrCircle2Circle2<Real>::GetIntersectionCircle () const
{
    return m_rkCircle0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrCircle2Circle2<float>;

// double
template WM3_ITEM
class IntrCircle2Circle2<double>;
//----------------------------------------------------------------------------
}
