// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistLine3Rectangle3.h"
#include "Wm3DistLine3Segment3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistLine3Rectangle3<Real>::DistLine3Rectangle3 (const Line3<Real>& rkLine,
    const Rectangle3<Real>& rkRectangle)
    :
    m_rkLine(rkLine),
    m_rkRectangle(rkRectangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistLine3Rectangle3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistLine3Rectangle3<Real>::GetRectangle () const
{
    return m_rkRectangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Rectangle3<Real>::GetSquared ()
{
    // compare line to all four edges of the rectangle
    Real fSqrDist = Math<Real>::MAX_REAL;
    for (int i1 = 0; i1 < 2; i1++)
    {
        for (int i0 = -1; i0 <= 1; i0 += 2)
        {
            Segment3<Real> kEdge;
            kEdge.Origin = m_rkRectangle.Center +
                (i0*m_rkRectangle.Extent[1-i1]) *
                m_rkRectangle.Axis[1-i1];
            kEdge.Direction = m_rkRectangle.Axis[i1];
            kEdge.Extent = m_rkRectangle.Extent[i1];
            DistLine3Segment3<Real> kLSDist(m_rkLine,kEdge);
            Real fSqrDistTmp = kLSDist.GetSquared();
            if ( fSqrDistTmp < fSqrDist )
            {
                m_kClosestPoint0 = kLSDist.GetClosestPoint0();
                m_kClosestPoint1 = kLSDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }
    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_rkLine.Direction);
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis,
        m_rkRectangle.Extent);
    return DistLine3Rectangle3<Real>(kMLine,kMRectangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkRectangle.Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_rkLine.Direction);
    Rectangle3<Real> kMRectangle(kMCenter,m_rkRectangle.Axis,
        m_rkRectangle.Extent);
    return DistLine3Rectangle3<Real>(kMLine,kMRectangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistLine3Rectangle3<float>;

// double
template WM3_ITEM
class DistLine3Rectangle3<double>;
//----------------------------------------------------------------------------
}
