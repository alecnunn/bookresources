// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrSphere3Cone3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrSphere3Cone3<Real>::IntrSphere3Cone3 (const Sphere3<Real>& rkSphere,
    const Cone3<Real>& rkCone)
    :
    m_rkSphere(rkSphere),
    m_rkCone(rkCone)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrSphere3Cone3<Real>::GetSphere () const
{
    return m_rkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
const Cone3<Real>& IntrSphere3Cone3<Real>::GetCone () const
{
    return m_rkCone;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Cone3<Real>::Test ()
{
    Real fInvSin = ((Real)1.0)/m_rkCone.SinAngle;
    Real fCosSqr = m_rkCone.CosAngle*m_rkCone.CosAngle;

    Vector3<Real> kCmV = m_rkSphere.Center - m_rkCone.Vertex;
    Vector3<Real> kD = kCmV + (m_rkSphere.Radius*fInvSin)*m_rkCone.Axis;
    Real fDSqrLen = kD.SquaredLength();
    Real fE = kD.Dot(m_rkCone.Axis);
    if ( fE > (Real)0.0 && fE*fE >= fDSqrLen*fCosSqr )
    {
        Real fSinSqr = m_rkCone.SinAngle*m_rkCone.SinAngle;
        fDSqrLen = kCmV.SquaredLength();
        fE = -kCmV.Dot(m_rkCone.Axis);
        if ( fE > (Real)0.0 && fE*fE >= fDSqrLen*fSinSqr )
        {
            Real fRSqr = m_rkSphere.Radius*m_rkSphere.Radius;
            return fDSqrLen <= fRSqr;
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Cone3<Real>::Find ()
{
    // test if cone vertex is in sphere
    Vector3<Real> kDiff = m_rkSphere.Center - m_rkCone.Vertex;
    Real fRSqr = m_rkSphere.Radius*m_rkSphere.Radius;
    Real fLSqr = kDiff.SquaredLength();
    if ( fLSqr <= fRSqr )
        return true;

    // test if sphere center is in cone
    Real fDot = kDiff.Dot(m_rkCone.Axis);
    Real fDotSqr = fDot*fDot;
    Real fCosSqr = m_rkCone.CosAngle*m_rkCone.CosAngle;
    if ( fDotSqr >= fLSqr*fCosSqr && fDot > (Real)0.0 )
    {
        // sphere center is inside cone, so sphere and cone intersect
        return true;
    }

    // Sphere center is outside cone.  Problem now reduces to looking for
    // an intersection between circle and ray in the plane containing
    // cone vertex and spanned by cone axis and vector from vertex to
    // sphere center.

    // Ray is t*D+V (t >= 0) where |D| = 1 and dot(A,D) = cos(angle).
    // Also, D = e*A+f*(C-V).  Plugging ray equation into sphere equation
    // yields R^2 = |t*D+V-C|^2, so the quadratic for intersections is
    // t^2 - 2*dot(D,C-V)*t + |C-V|^2 - R^2 = 0.  An intersection occurs
    // if and only if the discriminant is nonnegative.  This test becomes
    //
    //     dot(D,C-V)^2 >= dot(C-V,C-V) - R^2
    //
    // Note that if the right-hand side is nonpositive, then the inequality
    // is true (the sphere contains V).  I have already ruled this out in
    // the first block of code in this function.

    Real fULen = Math<Real>::Sqrt(Math<Real>::FAbs(fLSqr-fDotSqr));
    Real fTest = m_rkCone.CosAngle*fDot + m_rkCone.SinAngle*fULen;
    Real fDiscr = fTest*fTest - fLSqr + fRSqr;

    // compute point of intersection closest to vertex V
    Real fT = fTest - Math<Real>::Sqrt(fDiscr);
    Vector3<Real> kB = kDiff - fDot*m_rkCone.Axis;
    Real fTmp = m_rkCone.SinAngle/fULen;
    m_kPoint = fT*(m_rkCone.CosAngle*m_rkCone.Axis + fTmp*kB);

    return fDiscr >= (Real)0.0 && fTest >= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSphere3Cone3<Real>::GetPoint () const
{
    return m_kPoint;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrSphere3Cone3<float>;

// double
template WM3_ITEM
class IntrSphere3Cone3<double>;
//----------------------------------------------------------------------------
}
