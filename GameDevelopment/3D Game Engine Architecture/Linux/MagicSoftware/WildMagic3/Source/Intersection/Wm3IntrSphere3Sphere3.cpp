// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSphere3Sphere3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSphere3Sphere3<Real>::IntrSphere3Sphere3 (const Sphere3<Real>& rkSphere0,
    const Sphere3<Real>& rkSphere1)
    :
    m_rkSphere0(rkSphere0),
    m_rkSphere1(rkSphere1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrSphere3Sphere3<Real>::GetSphere0 () const
{
    return m_rkSphere0;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrSphere3Sphere3<Real>::GetSphere1 () const
{
    return m_rkSphere1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Sphere3<Real>::Test ()
{
    Vector3<Real> kDiff = m_rkSphere1.Center - m_rkSphere0.Center;
    Real fRSum = m_rkSphere0.Radius + m_rkSphere1.Radius;
    return kDiff.SquaredLength() <= fRSum*fRSum;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Sphere3<Real>::Find ()
{
    // plane of intersection must have N as its normal
    m_kNormal = m_rkSphere1.Center - m_rkSphere0.Center;
    Real fNSqrLen = m_kNormal.SquaredLength();
    Real fRSum = m_rkSphere0.Radius + m_rkSphere1.Radius;
    if ( fNSqrLen > fRSum*fRSum )
    {
        // sphere centers are too far apart for intersection
        return false;
    }

    Real fR0Sqr = m_rkSphere0.Radius*m_rkSphere0.Radius;
    Real fR1Sqr = m_rkSphere1.Radius*m_rkSphere1.Radius;
    Real fInvNSqrLen = ((Real)1.0)/fNSqrLen;
    Real fT = ((Real)0.5)*((Real)1.0+(fR0Sqr-fR1Sqr)*fInvNSqrLen);
    if ( fT < (Real)0.0 || fT > (Real)1.0 )
        return false;

    Real fRSqr = fR0Sqr - fT*fT*fNSqrLen;
    if ( fRSqr < (Real)0.0 )
        return false;

    // center and radius of circle of intersection
    m_kCenter = m_rkSphere0.Center + fT*m_kNormal;
    m_fRadius = Math<Real>::Sqrt(fRSqr);

    // compute U and V for plane of circle
    m_kNormal *= Math<Real>::Sqrt(fInvNSqrLen);
    Vector3<Real>::GenerateOrthonormalBasis(m_kUAxis,m_kVAxis,m_kNormal,true);

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Sphere3<Real>::Test (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kVDiff = rkVelocity1 - rkVelocity0;
    Real fA = kVDiff.SquaredLength();
    Vector3<Real> kCDiff = m_rkSphere1.Center - m_rkSphere0.Center;
    Real fC = kCDiff.SquaredLength();
    Real fRSum = m_rkSphere0.Radius + m_rkSphere1.Radius;
    Real fRSumSqr = fRSum*fRSum;

    if ( fA > (Real)0.0 )
    {
        Real fB = kCDiff.Dot(kVDiff);
        if ( fB <= (Real)0.0 )
        {
            if ( -fTMax*fA <= fB )
                return fA*fC - fB*fB <= fA*fRSumSqr;
            else
                return fTMax*(fTMax*fA + ((Real)2.0)*fB) + fC <= fRSumSqr;
        }
    }

    return fC <= fRSumSqr;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Sphere3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kVDiff = rkVelocity1 - rkVelocity0;
    Real fA = kVDiff.SquaredLength();
    Vector3<Real> kCDiff = m_rkSphere1.Center - m_rkSphere0.Center;
    Real fC = kCDiff.SquaredLength();
    Real fRSum = m_rkSphere0.Radius + m_rkSphere1.Radius;
    Real fRSumSqr = fRSum*fRSum;

    if ( fA > (Real)0.0 )
    {
        Real fB = kCDiff.Dot(kVDiff);
        if ( fB <= (Real)0.0 )
        {
            if ( -fTMax*fA <= fB
            ||   fTMax*(fTMax*fA + ((Real)2.0)*fB) + fC <= fRSumSqr )
            {
                Real fCDiff = fC - fRSumSqr;
                Real fDiscr = fB*fB - fA*fCDiff;
                if ( fDiscr >= (Real)0.0 )
                {
                    if ( fCDiff <= (Real)0.0 )
                    {
                        // The spheres are initially intersecting.  Estimate a
                        // point of contact by using the midpoint of the line
                        // segment connecting the sphere centers.
                        m_fContactTime = (Real)0.0;
                        m_kContactPoint = ((Real)0.5)*(m_rkSphere0.Center +
                            m_rkSphere1.Center);
                    }
                    else
                    {
                        // The first time of contact is in [0,fTMax].
                        m_fContactTime = -(fB + Math<Real>::Sqrt(fDiscr))/fA;
                        if ( m_fContactTime < (Real)0.0 )
                            m_fContactTime = (Real)0.0;
                        else if ( m_fContactTime > fTMax )
                            m_fContactTime = fTMax;

                        Vector3<Real> kNewCDiff = kCDiff +
                            m_fContactTime*kVDiff;

                        m_kContactPoint = m_rkSphere0.Center +
                            m_fContactTime*rkVelocity0 +
                            (m_rkSphere0.Radius/fRSum)*kNewCDiff;
                    }
                    return true;
                }
            }
            return false;
        }
    }

    if ( fC <= fRSumSqr )
    {
        // The spheres are initially intersecting.  Estimate a point of
        // contact by using the midpoint of the line segment connecting the
        // sphere centers.
        m_fContactTime = (Real)0.0;
        m_kContactPoint = ((Real)0.5)*(m_rkSphere0.Center +
            m_rkSphere1.Center);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSphere3Sphere3<Real>::GetCenter () const
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSphere3Sphere3<Real>::GetUAxis () const
{
    return m_kUAxis;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSphere3Sphere3<Real>::GetVAxis () const
{
    return m_kVAxis;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSphere3Sphere3<Real>::GetNormal () const
{
    return m_kNormal;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSphere3Sphere3<Real>::GetRadius () const
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSphere3Sphere3<Real>::GetContactPoint () const
{
    return m_kContactPoint;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrSphere3Sphere3<float>;

// double
template WM3_ITEM
class IntrSphere3Sphere3<double>;
//----------------------------------------------------------------------------
}
