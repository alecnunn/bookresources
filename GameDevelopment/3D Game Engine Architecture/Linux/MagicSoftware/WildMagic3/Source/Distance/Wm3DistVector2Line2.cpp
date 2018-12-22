// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistVector2Line2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistVector2Line2<Real>::DistVector2Line2 (const Vector2<Real>& rkVector,
    const Line2<Real>& rkLine)
    :
    m_rkVector(rkVector),
    m_rkLine(rkLine)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& DistVector2Line2<Real>::GetVector () const
{
    return m_rkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Line2<Real>& DistVector2Line2<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Line2<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Line2<Real>::GetSquared ()
{
    Vector2<Real> kDiff = m_rkVector - m_rkLine.Origin;
    Real fParam = m_rkLine.Direction.Dot(kDiff);
    m_kClosestPoint0 = m_rkVector;
    m_kClosestPoint1 = m_rkLine.Origin + fParam*m_rkLine.Direction;
    kDiff = m_kClosestPoint1 - m_kClosestPoint0;
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Line2<Real>::Get (Real fT, const Vector2<Real>& rkVelocity0,
    const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector2<Real> kMOrigin = m_rkLine.Origin + fT*rkVelocity1;
    Line2<Real> kMLine(kMOrigin,m_rkLine.Direction);
    return DistVector2Line2<Real>(kMVector,kMLine).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector2Line2<Real>::GetSquared (Real fT,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector2<Real> kMOrigin = m_rkLine.Origin + fT*rkVelocity1;
    Line2<Real> kMLine(kMOrigin,m_rkLine.Direction);
    return DistVector2Line2<Real>(kMVector,kMLine).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistVector2Line2<float>;

// double
template WM3_ITEM
class DistVector2Line2<double>;
//----------------------------------------------------------------------------
}
