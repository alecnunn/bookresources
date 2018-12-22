// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistLine3Circle3.h"
#include "Wm3PolynomialRoots.h"
#include "Wm3DistVector3Circle3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistLine3Circle3<Real>::DistLine3Circle3 (const Line3<Real>& rkLine,
    const Circle3<Real>& rkCircle)
    :
    m_rkLine(rkLine),
    m_rkCircle(rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistLine3Circle3<Real>::GetLine () const
{
    return m_rkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle3<Real>& DistLine3Circle3<Real>::GetCircle () const
{
    return m_rkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_rkLine.Origin - m_rkCircle.Center;
    Real fDSqrLen = kDiff.SquaredLength();
    Real fMdM = m_rkLine.Direction.SquaredLength();
    Real fDdM = kDiff.Dot(m_rkLine.Direction);
    Real fNdM = m_rkCircle.N.Dot(m_rkLine.Direction);
    Real fDdN = kDiff.Dot(m_rkCircle.N);

    Real fA0 = fDdM;
    Real fA1 = fMdM;
    Real fB0 = fDdM - fNdM*fDdN;
    Real fB1 = fMdM - fNdM*fNdM;
    Real fC0 = fDSqrLen - fDdN*fDdN;
    Real fC1 = fB0;
    Real fC2 = fB1;
    Real fRSqr = m_rkCircle.Radius*m_rkCircle.Radius;

    Real fA0Sqr = fA0*fA0;
    Real fA1Sqr = fA1*fA1;
    Real fTwoA0A1 = ((Real)2.0)*fA0*fA1;
    Real fB0Sqr = fB0*fB0;
    Real fB1Sqr = fB1*fB1;
    Real fTwoB0B1 = ((Real)2.0)*fB0*fB1;
    Real fTwoC1 = ((Real)2.0)*fC1;

    // The minimum point B+t*M occurs when t is a root of the quartic
    // equation whose coefficients are defined below.
    Polynomial1<Real> kPoly(4);
    kPoly[0] = fA0Sqr*fC0 - fB0Sqr*fRSqr;
    kPoly[1] = fTwoA0A1*fC0 + fA0Sqr*fTwoC1 - fTwoB0B1*fRSqr;
    kPoly[2] = fA1Sqr*fC0 + fTwoA0A1*fTwoC1 + fA0Sqr*fC2 - fB1Sqr*fRSqr;
    kPoly[3] = fA1Sqr*fTwoC1 + fTwoA0A1*fC2;
    kPoly[4] = fA1Sqr*fC2;

    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    kPR.FindB(kPoly,6);
    int iCount = kPR.GetCount();
    const Real* afRoot = kPR.GetRoots();

    Real fMinSqrDist = Math<Real>::MAX_REAL;
    for (int i = 0; i < iCount; i++)
    {
        // compute distance from P(t) to circle
        Vector3<Real> kP = m_rkLine.Origin + afRoot[i]*m_rkLine.Direction;
        DistVector3Circle3<Real> kVCDist(kP,m_rkCircle);
        Real fSqrDist = kVCDist.GetSquared();
        if ( fSqrDist < fMinSqrDist )
        {
            fMinSqrDist = fSqrDist;
            m_kClosestPoint0 = kVCDist.GetClosestPoint0();
            m_kClosestPoint1 = kVCDist.GetClosestPoint1();
        }
    }

    return fMinSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkCircle.Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_rkLine.Direction);
    Circle3<Real> kMCircle(kMCenter,m_rkCircle.U,m_rkCircle.V,
        m_rkCircle.N,m_rkCircle.Radius);
    return DistLine3Circle3<Real>(kMLine,kMCircle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_rkLine.Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_rkCircle.Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_rkLine.Direction);
    Circle3<Real> kMCircle(kMCenter,m_rkCircle.U,m_rkCircle.V,
        m_rkCircle.N,m_rkCircle.Radius);
    return DistLine3Circle3<Real>(kMLine,kMCircle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistLine3Circle3<float>;

// double
template WM3_ITEM
class DistLine3Circle3<double>;
//----------------------------------------------------------------------------
}
