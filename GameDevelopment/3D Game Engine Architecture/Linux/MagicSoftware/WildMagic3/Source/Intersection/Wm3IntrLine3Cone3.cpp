// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLine3Cone3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Cone3<Real>::IntrLine3Cone3 (const Line3<Real>& rkLine,
    const Cone3<Real>& rkCone)
    :
    m_rkLine(rkLine),
    m_rkCone(rkCone)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Cone3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Cone3<Real>& IntrLine3Cone3<Real>::GetCone () const
{
    return m_rkCone;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Cone3<Real>::Find ()
{
    // set up quadratic Q(t) = c2*t^2 + 2*c1*t + c0
    Real fAdD = m_rkCone.Axis.Dot(m_rkLine.Direction);
    Real fCosSqr = m_rkCone.CosAngle*m_rkCone.CosAngle;
    Vector3<Real> kE = m_rkLine.Origin - m_rkCone.Vertex;
    Real fAdE = m_rkCone.Axis.Dot(kE);
    Real fDdE = m_rkLine.Direction.Dot(kE);
    Real fEdE = kE.Dot(kE);
    Real fC2 = fAdD*fAdD - fCosSqr;
    Real fC1 = fAdD*fAdE - fCosSqr*fDdE;
    Real fC0 = fAdE*fAdE - fCosSqr*fEdE;

    // Solve the quadratic.  Keep only those X for which Dot(A,X-V) > 0.
    if ( Math<Real>::FAbs(fC2) >= Math<Real>::ZERO_TOLERANCE )
    {
        // c2 != 0
        Real fDiscr = fC1*fC1 - fC0*fC2;
        if ( fDiscr < (Real)0.0 )
        {
            // no real roots
            m_iIntersectionType = IT_EMPTY;
            m_iQuantity = 0;
        }
        else if ( fDiscr > Math<Real>::ZERO_TOLERANCE )
        {
            // two distinct real roots
            Real fRoot = Math<Real>::Sqrt(fDiscr);
            Real fInvC2 = ((Real)1.0)/fC2;
            m_iQuantity = 0;

            Real fT = (-fC1 - fRoot)*fInvC2;
            m_akPoint[m_iQuantity] = m_rkLine.Origin + fT*m_rkLine.Direction;
            kE = m_akPoint[m_iQuantity] - m_rkCone.Vertex;
            if ( kE.Dot(m_rkCone.Axis) > (Real)0.0 )
                m_iQuantity++;

            fT = (-fC1 + fRoot)*fInvC2;
            m_akPoint[m_iQuantity] = m_rkLine.Origin + fT*m_rkLine.Direction;
            kE = m_akPoint[m_iQuantity] - m_rkCone.Vertex;
            if ( kE.Dot(m_rkCone.Axis) > (Real)0.0 )
                m_iQuantity++;

            if ( m_iQuantity == 2 )
            {
                m_iIntersectionType = IT_SEGMENT;
            }
            else if ( m_iQuantity == 1 )
            {
                m_iIntersectionType = IT_RAY;
                m_akPoint[m_iQuantity++] = m_rkLine.Direction;
            }
            else
            {
                assert( false );
                m_iIntersectionType = IT_EMPTY;
            }
        }
        else
        {
            // one repeated real root (line is tangent to the cone)
            m_akPoint[0] = m_rkLine.Origin - (fC1/fC2)*m_rkLine.Direction;
            kE = m_akPoint[0] - m_rkCone.Vertex;
            if ( kE.Dot(m_rkCone.Axis) > (Real)0.0 )
            {
                m_iIntersectionType = IT_POINT;
                m_iQuantity = 1;
            }
            else
            {
                m_iIntersectionType = IT_EMPTY;
                m_iQuantity = 0;
            }
        }
    }
    else if ( Math<Real>::FAbs(fC1) >= Math<Real>::ZERO_TOLERANCE )
    {
        // c2 = 0, c1 != 0 (D is a direction vector on the cone boundary)
        m_akPoint[0] = m_rkLine.Origin - 
            (((Real)0.5)*fC0/fC1)*m_rkLine.Direction;
        kE = m_akPoint[0] - m_rkCone.Vertex;
        if ( kE.Dot(m_rkCone.Axis) > (Real)0.0 )
        {
            m_iIntersectionType = IT_RAY;
            m_iQuantity = 2;
            m_akPoint[1] = m_rkLine.Direction;
        }
        else
        {
            m_iIntersectionType = IT_EMPTY;
            m_iQuantity = 0;
        }
    }
    else if ( Math<Real>::FAbs(fC0) >= Math<Real>::ZERO_TOLERANCE )
    {
        // c2 = c1 = 0, c0 != 0
        m_iIntersectionType = IT_EMPTY;
        m_iQuantity = 0;
    }
    else
    {
        // c2 = c1 = c0 = 0, cone contains ray V+t*D where V is cone vertex
        // and D is the line direction.
        m_iIntersectionType = IT_RAY;
        m_iQuantity = 2;
        m_akPoint[0] = m_rkCone.Vertex;
        m_akPoint[1] = m_rkLine.Direction;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Cone3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Cone3<Real>::GetPoint (int i) const
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
class IntrLine3Cone3<float>;

// double
template WM3_ITEM
class IntrLine3Cone3<double>;
//----------------------------------------------------------------------------
}
