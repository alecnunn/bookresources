// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLine2Box2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine2Box2<Real>::IntrLine2Box2 (const Line2<Real>& rkLine,
    const Box2<Real>& rkBox)
    :
    m_rkLine(rkLine),
    m_rkBox(rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line2<Real>& IntrLine2Box2<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Box2<Real>& IntrLine2Box2<Real>::GetBox () const
{
    return m_rkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine2Box2<Real>::Test ()
{
    Vector2<Real> kDiff = m_rkLine.Origin - m_rkBox.Center;
    Vector2<Real> kPerp = m_rkLine.Direction.Perp();
    Real fLhs = Math<Real>::FAbs(kPerp.Dot(kDiff));
    Real fPart0 = Math<Real>::FAbs(kPerp.Dot(m_rkBox.Axis[0]));
    Real fPart1 = Math<Real>::FAbs(kPerp.Dot(m_rkBox.Axis[1]));
    Real fRhs = m_rkBox.Extent[0]*fPart0 + m_rkBox.Extent[1]*fPart1;
    return fLhs <= fRhs;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine2Box2<Real>::Find ()
{
    Real fT0 = -Math<Real>::MAX_REAL, fT1 = Math<Real>::MAX_REAL;
    return DoClipping(fT0,fT1,m_rkLine.Origin,m_rkLine.Direction,m_rkBox,
        m_iQuantity,m_akPoint,m_iIntersectionType);
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine2Box2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrLine2Box2<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine2Box2<Real>::DoClipping (Real fT0, Real fT1,
    const Vector2<Real>& rkOrigin, const Vector2<Real>& rkDirection,
    const Box2<Real>& rkBox, int& riQuantity, Vector2<Real> akPoint[2],
    int& riIntrType)
{
    assert( fT0 < fT1 );

    // convert linear component to box coordinates
    Vector2<Real> kDiff = rkOrigin - rkBox.Center;
    Vector2<Real> kBOrigin(
        kDiff.Dot(rkBox.Axis[0]),
        kDiff.Dot(rkBox.Axis[1])
    );
    Vector2<Real> kBDirection(
        rkDirection.Dot(rkBox.Axis[0]),
        rkDirection.Dot(rkBox.Axis[1])
    );

    Real fSaveT0 = fT0, fSaveT1 = fT1;
    bool bNotAllClipped =
        Clip(+kBDirection.X(),-kBOrigin.X()-rkBox.Extent[0],fT0,fT1) &&
        Clip(-kBDirection.X(),+kBOrigin.X()-rkBox.Extent[0],fT0,fT1) &&
        Clip(+kBDirection.Y(),-kBOrigin.Y()-rkBox.Extent[1],fT0,fT1) &&
        Clip(-kBDirection.Y(),+kBOrigin.Y()-rkBox.Extent[1],fT0,fT1);

    if ( bNotAllClipped && (fT0 != fSaveT0 || fT1 != fSaveT1) )
    {
        if ( fT1 > fT0 )
        {
            riIntrType = IT_SEGMENT;
            riQuantity = 2;
            akPoint[0] = rkOrigin + fT0*rkDirection;
            akPoint[1] = rkOrigin + fT1*rkDirection;
        }
        else
        {
            riIntrType = IT_POINT;
            riQuantity = 1;
            akPoint[0] = rkOrigin + fT0*rkDirection;
        }
    }
    else
    {
        riIntrType = IT_EMPTY;
        riQuantity = 0;
    }

    return riIntrType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine2Box2<Real>::Clip (Real fDenom, Real fNumer, Real& rfT0,
    Real& rfT1)
{
    // Return value is 'true' if line segment intersects the current test
    // plane.  Otherwise 'false' is returned in which case the line segment
    // is entirely clipped.

    if ( fDenom > (Real)0.0 )
    {
        if ( fNumer > fDenom*rfT1 )
            return false;
        if ( fNumer > fDenom*rfT0 )
            rfT0 = fNumer/fDenom;
        return true;
    }
    else if ( fDenom < (Real)0.0 )
    {
        if ( fNumer > fDenom*rfT0 )
            return false;
        if ( fNumer > fDenom*rfT1 )
            rfT1 = fNumer/fDenom;
        return true;
    }
    else
    {
        return fNumer <= (Real)0.0;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrLine2Box2<float>;

// double
template WM3_ITEM
class IntrLine2Box2<double>;
//----------------------------------------------------------------------------
}
