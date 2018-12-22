// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistVector3Frustum3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Frustum3<Real>::DistVector3Frustum3 (const Vector3<Real>& rkVector,
    const Frustum3<Real>& rkFrustum)
    :
    m_rkVector(rkVector),
    m_rkFrustum(rkFrustum)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Frustum3<Real>::GetVector () const
{
    return m_rkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Frustum3<Real>& DistVector3Frustum3<Real>::GetFrustum () const
{
    return m_rkFrustum;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::GetSquared ()
{
    // compute coordinates of point with respect to frustum coordinate system
    Vector3<Real> kDiff = m_rkVector - m_rkFrustum.Origin;
    Vector3<Real> kTest = Vector3<Real>(
        kDiff.Dot(m_rkFrustum.RVector),
        kDiff.Dot(m_rkFrustum.UVector),
        kDiff.Dot(m_rkFrustum.DVector));

    // perform calculations in octant with nonnegative R and U coordinates
    bool bRSignChange;
    if ( kTest.X() < (Real)0.0 )
    {
        bRSignChange = true;
        kTest.X() = -kTest.X();
    }
    else
    {
        bRSignChange = false;
    }

    bool bUSignChange;
    if ( kTest.Y() < (Real)0.0 )
    {
        bUSignChange = true;
        kTest.Y() = -kTest.Y();
    }
    else
    {
        bUSignChange = false;
    }

    // frustum derived parameters
    Real fRMin = m_rkFrustum.RBound;
    Real fRMax = m_rkFrustum.GetDRatio()*fRMin;
    Real fUMin = m_rkFrustum.UBound;
    Real fUMax = m_rkFrustum.GetDRatio()*fUMin;
    Real fDMin = m_rkFrustum.DMin;
    Real fDMax = m_rkFrustum.DMax;
    Real fRMinSqr = fRMin*fRMin;
    Real fUMinSqr = fUMin*fUMin;
    Real fDMinSqr = fDMin*fDMin;
    Real fMinRDDot = fRMinSqr + fDMinSqr;
    Real fMinUDDot = fUMinSqr + fDMinSqr;
    Real fMinRUDDot = fRMinSqr + fMinUDDot;
    Real fMaxRDDot = m_rkFrustum.GetDRatio()*fMinRDDot;
    Real fMaxUDDot = m_rkFrustum.GetDRatio()*fMinUDDot;
    Real fMaxRUDDot = m_rkFrustum.GetDRatio()*fMinRUDDot;

    // Algorithm computes closest point in all cases by determining in which
    // Voronoi region of the vertices, edges, and faces of the frustum that
    // the test point lives.
    Vector3<Real> kClosest;
    Real fRDot, fUDot, fRDDot, fUDDot, fRUDDot, fREdgeDot, fUEdgeDot, fT;
    if ( kTest.Z() >= fDMax )
    {
        if ( kTest.X() <= fRMax )
        {
            if ( kTest.Y() <= fUMax )
            {
                // F-face
                kClosest.X() = kTest.X();
                kClosest.Y() = kTest.Y();
                kClosest.Z() = fDMax;
            }
            else
            {
                // UF-edge
                kClosest.X() = kTest.X();
                kClosest.Y() = fUMax;
                kClosest.Z() = fDMax;
            }
        }
        else
        {
            if ( kTest.Y() <= fUMax )
            {
                // LF-edge
                kClosest.X() = fRMax;
                kClosest.Y() = kTest.Y();
                kClosest.Z() = fDMax;
            }
            else
            {
                // LUF-vertex
                kClosest.X() = fRMax;
                kClosest.Y() = fUMax;
                kClosest.Z() = fDMax;
            }
        }
    }
    else if ( kTest.Z() <= fDMin )
    {
        if ( kTest.X() <= fRMin )
        {
            if ( kTest.Y() <= fUMin )
            {
                // N-face
                kClosest.X() = kTest.X();
                kClosest.Y() = kTest.Y();
                kClosest.Z() = fDMin;
            }
            else
            {
                fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                if ( fUDDot >= fMaxUDDot )
                {
                    // UF-edge
                    kClosest.X() = kTest.X();
                    kClosest.Y() = fUMax;
                    kClosest.Z() = fDMax;
                }
                else if ( fUDDot >= fMinUDDot )
                {
                    // U-face
                    fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                    fT = fUDot/fMinUDDot;
                    kClosest.X() = kTest.X();
                    kClosest.Y() = kTest.Y() - fT*fDMin;
                    kClosest.Z() = kTest.Z() + fT*fUMin;
                }
                else
                {
                    // UN-edge
                    kClosest.X() = kTest.X();
                    kClosest.Y() = fUMin;
                    kClosest.Z() = fDMin;
                }
            }
        }
        else
        {
            if ( kTest.Y() <= fUMin )
            {
                fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                if ( fRDDot >= fMaxRDDot )
                {
                    // LF-edge
                    kClosest.X() = fRMax;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = fDMax;
                }
                else if ( fRDDot >= fMinRDDot )
                {
                    // L-face
                    fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                    fT = fRDot/fMinRDDot;
                    kClosest.X() = kTest.X() - fT*fDMin;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = kTest.Z() + fT*fRMin;
                }
                else
                {
                    // LN-edge
                    kClosest.X() = fRMin;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = fDMin;
                }
            }
            else
            {
                fRUDDot = fRMin*kTest.X() + fUMin*kTest.Y() + fDMin*kTest.Z();
                fREdgeDot = fUMin*fRUDDot - fMinRUDDot*kTest.Y();
                if ( fREdgeDot >= (Real)0.0 )
                {
                    fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                    if ( fRDDot >= fMaxRDDot )
                    {
                        // LF-edge
                        kClosest.X() = fRMax;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = fDMax;
                    }
                    else if ( fRDDot >= fMinRDDot )
                    {
                        // L-face
                        fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                        fT = fRDot/fMinRDDot;
                        kClosest.X() = kTest.X() - fT*fDMin;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = kTest.Z() + fT*fRMin;
                    }
                    else
                    {
                        // LN-edge
                        kClosest.X() = fRMin;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = fDMin;
                    }
                }
                else
                {
                    fUEdgeDot = fRMin*fRUDDot - fMinRUDDot*kTest.X();
                    if ( fUEdgeDot >= (Real)0.0 )
                    {
                        fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                        if ( fUDDot >= fMaxUDDot )
                        {
                            // UF-edge
                            kClosest.X() = kTest.X();
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else if ( fUDDot >= fMinUDDot )
                        {
                            // U-face
                            fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                            fT = fUDot/fMinUDDot;
                            kClosest.X() = kTest.X();
                            kClosest.Y() = kTest.Y() - fT*fDMin;
                            kClosest.Z() = kTest.Z() + fT*fUMin;
                        }
                        else
                        {
                            // UN-edge
                            kClosest.X() = kTest.X();
                            kClosest.Y() = fUMin;
                            kClosest.Z() = fDMin;
                        }
                    }
                    else
                    {
                        if ( fRUDDot >= fMaxRUDDot )
                        {
                            // LUF-vertex
                            kClosest.X() = fRMax;
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else if ( fRUDDot >= fMinRUDDot )
                        {
                            // LU-edge
                            fT = fRUDDot/fMinRUDDot;
                            kClosest.X() = fT*fRMin;
                            kClosest.Y() = fT*fUMin;
                            kClosest.Z() = fT*fDMin;
                        }
                        else
                        {
                            // LUN-vertex
                            kClosest.X() = fRMin;
                            kClosest.Y() = fUMin;
                            kClosest.Z() = fDMin;
                        }
                    }
                }
            }
        }
    }
    else
    {
        fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
        fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
        if ( fRDot <= (Real)0.0 )
        {
            if ( fUDot <= (Real)0.0 )
            {
                // point inside frustum
                kClosest = kTest;
            }
            else
            {
                fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                if ( fUDDot >= fMaxUDDot )
                {
                    // UF-edge
                    kClosest.X() = kTest.X();
                    kClosest.Y() = fUMax;
                    kClosest.Z() = fDMax;
                }
                else
                {
                    // U-face
                    fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                    fT = fUDot/fMinUDDot;
                    kClosest.X() = kTest.X();
                    kClosest.Y() = kTest.Y() - fT*fDMin;
                    kClosest.Z() = kTest.Z() + fT*fUMin;
                }
            }
        }
        else
        {
            if ( fUDot <= (Real)0.0 )
            {
                fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                if ( fRDDot >= fMaxRDDot )
                {
                    // LF-edge
                    kClosest.X() = fRMax;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = fDMax;
                }
                else
                {
                    // L-face
                    fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                    fT = fRDot/fMinRDDot;
                    kClosest.X() = kTest.X() - fT*fDMin;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = kTest.Z() + fT*fRMin;
                }
            }
            else
            {
                fRUDDot = fRMin*kTest.X() + fUMin*kTest.Y() + fDMin*kTest.Z();
                fREdgeDot = fUMin*fRUDDot - fMinRUDDot*kTest.Y();
                if ( fREdgeDot >= (Real)0.0 )
                {
                    fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                    if ( fRDDot >= fMaxRDDot )
                    {
                        // LF-edge
                        kClosest.X() = fRMax;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = fDMax;
                    }
                    else // assert( fRDDot >= fMinRDDot ) from geometry
                    {
                        // L-face
                        fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                        fT = fRDot/fMinRDDot;
                        kClosest.X() = kTest.X() - fT*fDMin;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = kTest.Z() + fT*fRMin;
                    }
                }
                else
                {
                    fUEdgeDot = fRMin*fRUDDot - fMinRUDDot*kTest.X();
                    if ( fUEdgeDot >= (Real)0.0 )
                    {
                        fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                        if ( fUDDot >= fMaxUDDot )
                        {
                            // UF-edge
                            kClosest.X() = kTest.X();
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else // assert( fUDDot >= fMinUDDot ) from geometry
                        {
                            // U-face
                            fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                            fT = fUDot/fMinUDDot;
                            kClosest.X() = kTest.X();
                            kClosest.Y() = kTest.Y() - fT*fDMin;
                            kClosest.Z() = kTest.Z() + fT*fUMin;
                        }
                    }
                    else
                    {
                        if ( fRUDDot >= fMaxRUDDot )
                        {
                            // LUF-vertex
                            kClosest.X() = fRMax;
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else // assert( fRUDDot >= fMinRUDDot ) from geometry
                        {
                            // LU-edge
                            fT = fRUDDot/fMinRUDDot;
                            kClosest.X() = fT*fRMin;
                            kClosest.Y() = fT*fUMin;
                            kClosest.Z() = fT*fDMin;
                        }
                    }
                }
            }
        }
    }

    kDiff = kTest - kClosest;

    // convert back to original quadrant
    if ( bRSignChange )
        kClosest.X() = -kClosest.X();

    if ( bUSignChange )
        kClosest.Y() = -kClosest.Y();

    m_kClosestPoint0 = m_rkVector;

    // convert back to original coordinates
    m_kClosestPoint1 = m_rkFrustum.Origin +
        kClosest.X()*m_rkFrustum.RVector +
        kClosest.Y()*m_rkFrustum.UVector +
        kClosest.Z()*m_rkFrustum.DVector;

    // compute and return squared distance
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_rkFrustum.Origin + fT*rkVelocity1;
    Frustum3<Real> kMFrustum(kMOrigin,m_rkFrustum.DVector,
        m_rkFrustum.UVector,m_rkFrustum.RVector,m_rkFrustum.DMin,
        m_rkFrustum.DMax,m_rkFrustum.UBound,m_rkFrustum.RBound);
    return DistVector3Frustum3<Real>(kMVector,kMFrustum).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = m_rkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_rkFrustum.Origin + fT*rkVelocity1;
    Frustum3<Real> kMFrustum(kMOrigin,m_rkFrustum.DVector,
        m_rkFrustum.UVector,m_rkFrustum.RVector,m_rkFrustum.DMin,
        m_rkFrustum.DMax,m_rkFrustum.UBound,m_rkFrustum.RBound);
    return DistVector3Frustum3<Real>(kMVector,kMFrustum).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistVector3Frustum3<float>;

// double
template WM3_ITEM
class DistVector3Frustum3<double>;
//----------------------------------------------------------------------------
}
