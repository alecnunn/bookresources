// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrTriangle2Triangle2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrTriangle2Triangle2<Real>::IntrTriangle2Triangle2 (
    const Triangle2<Real>& rkTriangle0, const Triangle2<Real>& rkTriangle1)
    :
    m_rkTriangle0(rkTriangle0),
    m_rkTriangle1(rkTriangle1)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle2<Real>& IntrTriangle2Triangle2<Real>::GetTriangle0 () const
{
    return m_rkTriangle0;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle2<Real>& IntrTriangle2Triangle2<Real>::GetTriangle1 () const
{
    return m_rkTriangle1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle2Triangle2<Real>::Test ()
{
    int i0, i1;
    Vector2<Real> kDir;

    // test edges of triangle0 for separation
    for (i0 = 0, i1 = 2; i0 < 3; i1 = i0, i0++)
    {
        // test axis V0[i1] + t*perp(V0[i0]-V0[i1]), perp(x,y) = (y,-x)
        kDir.X() = m_rkTriangle0.V[i0].Y() - m_rkTriangle0.V[i1].Y();
        kDir.Y() = m_rkTriangle0.V[i1].X() - m_rkTriangle0.V[i0].X();
        if ( WhichSide(m_rkTriangle1.V,m_rkTriangle0.V[i1],kDir) > 0 )
        {
            // triangle1 is entirely on positive side of triangle0 edge
            return false;
        }
    }

    // test edges of triangle1 for separation
    for (i0 = 0, i1 = 2; i0 < 3; i1 = i0, i0++)
    {
        // test axis V1[i1] + t*perp(V1[i0]-V1[i1]), perp(x,y) = (y,-x)
        kDir.X() = m_rkTriangle1.V[i0].Y() - m_rkTriangle1.V[i1].Y();
        kDir.Y() = m_rkTriangle1.V[i1].X() - m_rkTriangle1.V[i0].X();
        if ( WhichSide(m_rkTriangle0.V,m_rkTriangle1.V[i1],kDir) > 0 )
        {
            // triangle0 is entirely on positive side of triangle1 edge
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle2Triangle2<Real>::Find ()
{
    // The potential intersection is initialized to triangle1.  The set of
    // vertices is refined based on clipping against each edge of triangle0.
    m_iQuantity = 3;
    for (int i = 0; i < 3; i++)
        m_akPoint[i] = m_rkTriangle1.V[i];

    for (int i1 = 2, i0 = 0; i0 < 3; i1 = i0, i0++)
    {
        // clip against edge <V0[i1],V0[i0]>
        Vector2<Real> kN(
            m_rkTriangle0.V[i1].Y() - m_rkTriangle0.V[i0].Y(),
            m_rkTriangle0.V[i0].X() - m_rkTriangle0.V[i1].X());
        Real fC = kN.Dot(m_rkTriangle0.V[i1]);
        ClipConvexPolygonAgainstLine(kN,fC,m_iQuantity,m_akPoint);
        if ( m_iQuantity == 0 )
        {
            // triangle completely clipped, no intersection occurs
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle2Triangle2<Real>::Test (Real fTMax,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    // process as if V0-triangle is stationary and V1-triangle is moving
    Vector2<Real> kW = rkVelocity1 - rkVelocity0;
    int iSide = 0;  // 0 = NONE, -1 = LEFT, +1 = RIGHT
    Real fTFirst = (Real)0.0;
    Real fTLast = Math<Real>::MAX_REAL;

    Configuration kCfg0, kCfg1, kTCfg0, kTCfg1;
    int i0, i1, i2;
    Vector2<Real> kD;
    Real fSpeed;

    // process edges of V0-triangle
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2, i2++)
    {
        // test axis V0[i1] + t*perp(V0[i2]-V0[i1]), perp(x,y) = (y,-x)
        kD.X() = m_rkTriangle0.V[i2].Y() - m_rkTriangle0.V[i1].Y();
        kD.Y() = m_rkTriangle0.V[i1].X() - m_rkTriangle0.V[i2].X();
        fSpeed = kD.Dot(kW);

        kCfg0.ComputeTwo(m_rkTriangle0.V,kD,i0,i1,i2);
        kCfg1.ComputeThree(m_rkTriangle1.V,kD,m_rkTriangle0.V[i1]);

        if ( Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,fTFirst,fTLast) )
        {
            return false;
        }
    }

    // process edges of V1-triangle
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2, i2++)
    {
        // test axis V1[i1] + t*perp(V1[i2]-V1[i1]), perp(x,y) = (y,-x)
        kD.X() = m_rkTriangle1.V[i2].Y() - m_rkTriangle1.V[i1].Y();
        kD.Y() = m_rkTriangle1.V[i1].X() - m_rkTriangle1.V[i2].X();
        fSpeed = kD.Dot(kW);

        kCfg1.ComputeTwo(m_rkTriangle1.V,kD,i0,i1,i2);
        kCfg0.ComputeThree(m_rkTriangle0.V,kD,m_rkTriangle1.V[i1]);

        if ( Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,fTFirst,fTLast) )
        {
            return false;
        }
    }

    m_fContactTime = fTFirst;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle2Triangle2<Real>::Find (Real fTMax,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    // process as if V0-triangle is stationary and V1-triangle is moving
    Vector2<Real> kW = rkVelocity1 - rkVelocity0;
    int iSide = 0;  // 0 = NONE, -1 = LEFT, +1 = RIGHT
    Real fTFirst = (Real)0.0;
    Real fTLast = Math<Real>::MAX_REAL;

    Configuration kCfg0, kCfg1, kTCfg0, kTCfg1;
    int i0, i1, i2;
    Vector2<Real> kD;
    Real fSpeed;

    // process edges of V0-triangle
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2, i2++)
    {
        // test axis V0[i1] + t*perp(V0[i2]-V0[i1]), perp(x,y) = (y,-x)
        kD.X() = m_rkTriangle0.V[i2].Y() - m_rkTriangle0.V[i1].Y();
        kD.Y() = m_rkTriangle0.V[i1].X() - m_rkTriangle0.V[i2].X();
        fSpeed = kD.Dot(kW);

        kCfg0.ComputeTwo(m_rkTriangle0.V,kD,i0,i1,i2);
        kCfg1.ComputeThree(m_rkTriangle1.V,kD,m_rkTriangle0.V[i1]);

        if ( Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,fTFirst,fTLast) )
        {
            return false;
        }
    }

    // process edges of V1-triangle
    for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2, i2++)
    {
        // test axis V1[i1] + t*perp(V1[i2]-V1[i1]), perp(x,y) = (y,-x)
        kD.X() = m_rkTriangle1.V[i2].Y() - m_rkTriangle1.V[i1].Y();
        kD.Y() = m_rkTriangle1.V[i1].X() - m_rkTriangle1.V[i2].X();
        fSpeed = kD.Dot(kW);

        kCfg1.ComputeTwo(m_rkTriangle1.V,kD,i0,i1,i2);
        kCfg0.ComputeThree(m_rkTriangle0.V,kD,m_rkTriangle1.V[i1]);

        if ( Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,fTFirst,fTLast) )
        {
            return false;
        }
    }

    // move triangles to first contact
    Vector2<Real> akMoveV0[3], akMoveV1[3];
    for (int i = 0; i < 3; i++)
    {
        akMoveV0[i] = m_rkTriangle0.V[i] + fTFirst*rkVelocity0;
        akMoveV1[i] = m_rkTriangle1.V[i] + fTFirst*rkVelocity1;
    };

    Configuration::GetIntersection(kTCfg0,kTCfg1,iSide,akMoveV0,akMoveV1,
        m_iQuantity,m_akPoint);

    m_fContactTime = fTFirst;
    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrTriangle2Triangle2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrTriangle2Triangle2<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
int IntrTriangle2Triangle2<Real>::WhichSide (const Vector2<Real> akV[3],
    const Vector2<Real>& rkP, const Vector2<Real>& rkD)
{
    // Vertices are projected to the form P+t*D.  Return value is +1 if all
    // t > 0, -1 if all t < 0, 0 otherwise, in which case the line splits the
    // triangle.

    int iPositive = 0, iNegative = 0, iZero = 0;
    for (int i = 0; i < 3; i++)
    {
        Real fT = rkD.Dot(akV[i] - rkP);
        if ( fT > (Real)0.0 )
            iPositive++;
        else if ( fT < (Real)0.0 )
            iNegative++;
        else
            iZero++;

        if ( iPositive > 0 && iNegative > 0 )
            return 0;
    }
    return ( iZero == 0 ? ( iPositive > 0 ? 1 : -1 ) : 0 );
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle2Triangle2<Real>::ClipConvexPolygonAgainstLine (
    const Vector2<Real>& rkN, Real fC, int& riQuantity,
    Vector2<Real> akV[6])
{
    // The input vertices are assumed to be in counterclockwise order.  The
    // ordering is an invariant of this function.

    // test on which side of line the vertices are
    int iPositive = 0, iNegative = 0, iPIndex = -1;
    Real afTest[6];
    int i;
    for (i = 0; i < riQuantity; i++)
    {
        afTest[i] = rkN.Dot(akV[i]) - fC;
        if ( afTest[i] > (Real)0.0 )
        {
            iPositive++;
            if ( iPIndex < 0 )
                iPIndex = i;
        }
        else if ( afTest[i] < (Real)0.0 )
        {
            iNegative++;
        }
    }

    if ( iPositive > 0 )
    {
        if ( iNegative > 0 )
        {
            // line transversely intersects polygon
            Vector2<Real> akCV[6];
            int iCQuantity = 0, iCur, iPrv;
            Real fT;

            if ( iPIndex > 0 )
            {
                // first clip vertex on line
                iCur = iPIndex;
                iPrv = iCur-1;
                fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);

                // vertices on positive side of line
                while ( iCur < riQuantity && afTest[iCur] > (Real)0.0 )
                    akCV[iCQuantity++] = akV[iCur++];

                // last clip vertex on line
                if ( iCur < riQuantity )
                {
                    iPrv = iCur-1;
                }
                else
                {
                    iCur = 0;
                    iPrv = riQuantity - 1;
                }
                fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);
            }
            else  // iPIndex is 0
            {
                // vertices on positive side of line
                iCur = 0;
                while ( iCur < riQuantity && afTest[iCur] > (Real)0.0 )
                    akCV[iCQuantity++] = akV[iCur++];

                // last clip vertex on line
                iPrv = iCur-1;
                fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);

                // skip vertices on negative side
                while ( iCur < riQuantity && afTest[iCur] <= (Real)0.0 )
                    iCur++;

                // first clip vertex on line
                if ( iCur < riQuantity )
                {
                    iPrv = iCur-1;
                    fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                    akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);

                    // vertices on positive side of line
                    while ( iCur < riQuantity && afTest[iCur] > (Real)0.0 )
                        akCV[iCQuantity++] = akV[iCur++];
                }
                else
                {
                    // iCur = 0
                    iPrv = riQuantity - 1;
                    fT = afTest[0]/(afTest[0] - afTest[iPrv]);
                    akCV[iCQuantity++] = akV[0]+fT*(akV[iPrv]-akV[0]);
                }
            }

            riQuantity = iCQuantity;
            memcpy(akV,akCV,iCQuantity*sizeof(Vector2<Real>));
        }
        // else polygon fully on positive side of line, nothing to do
    }
    else
    {
        // polygon does not intersect positive side of line, clip all
        riQuantity = 0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle2Triangle2<Real>::Configuration::ComputeTwo (
    const Vector2<Real> akV[3], const Vector2<Real>& rkD, int i0, int i1,
    int i2)
{
    m_eMap = M12;
    m_aiIndex[0] = i0;
    m_aiIndex[1] = i1;
    m_aiIndex[2] = i2;
    m_fMin = rkD.Dot(akV[i0] - akV[i1]);
    m_fMax = (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle2Triangle2<Real>::Configuration::ComputeThree (
    const Vector2<Real> akV[3], const Vector2<Real>& rkD,
    const Vector2<Real>& rkP)
{
    Real fD0 = rkD.Dot(akV[0] - rkP);
    Real fD1 = rkD.Dot(akV[1] - rkP);
    Real fD2 = rkD.Dot(akV[2] - rkP);

    // Make sure that m_aiIndex[...] is an even permutation of (0,1,2)
    // whenever the map value is M12 or M21.  This is needed to guarantee
    // the intersection of overlapping edges is properly computed.

    if ( fD0 <= fD1 )
    {
        if ( fD1 <= fD2 )  // d0 <= d1 <= d2
        {
            if ( fD0 != fD1 )
                m_eMap = ( fD1 != fD2 ? M11 : M12 );
            else
                m_eMap = M21;

            m_aiIndex[0] = 0;
            m_aiIndex[1] = 1;
            m_aiIndex[2] = 2;
            m_fMin = fD0;
            m_fMax = fD2;
        }
        else if ( fD0 <= fD2 )  // d0 <= d2 < d1
        {
            if ( fD0 != fD2 )
            {
                m_eMap = M11;
                m_aiIndex[0] = 0;
                m_aiIndex[1] = 2;
                m_aiIndex[2] = 1;
            }
            else
            {
                m_eMap = M21;
                m_aiIndex[0] = 2;
                m_aiIndex[1] = 0;
                m_aiIndex[2] = 1;
            }

            m_fMin = fD0;
            m_fMax = fD1;
        }
        else  // d2 < d0 <= d1
        {
            m_eMap = ( fD0 != fD1 ? M12 : M11 );
            m_aiIndex[0] = 2;
            m_aiIndex[1] = 0;
            m_aiIndex[2] = 1;
            m_fMin = fD2;
            m_fMax = fD1;
        }
    }
    else
    {
        if ( fD2 <= fD1 )  // d2 <= d1 < d0
        {
            if ( fD2 != fD1 )
            {
                m_eMap = M11;
                m_aiIndex[0] = 2;
                m_aiIndex[1] = 1;
                m_aiIndex[2] = 0;
            }
            else
            {
                m_eMap = M21;
                m_aiIndex[0] = 1;
                m_aiIndex[1] = 2;
                m_aiIndex[2] = 0;
            }

            m_fMin = fD2;
            m_fMax = fD0;
        }
        else if ( fD2 <= fD0 )  // d1 < d2 <= d0
        {
            m_eMap = ( fD2 != fD0 ? M11 : M12 );
            m_aiIndex[0] = 1;
            m_aiIndex[1] = 2;
            m_aiIndex[2] = 0;
            m_fMin = fD1;
            m_fMax = fD0;
        }
        else  // d1 < d0 < d2
        {
            m_eMap = M11;
            m_aiIndex[0] = 1;
            m_aiIndex[1] = 0;
            m_aiIndex[2] = 2;
            m_fMin = fD1;
            m_fMax = fD2;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle2Triangle2<Real>::Configuration::NoIntersect (
    const Configuration& rkCfg0, const Configuration& rkCfg1, Real fTMax,
    Real fSpeed, int& riSide, Configuration& rkTCfg0, Configuration& rkTCfg1,
    Real& rfTFirst, Real& rfTLast)
{
    Real fInvSpeed, fT;

    if ( rkCfg1.m_fMax < rkCfg0.m_fMin )
    {
        // V1-interval initially on left of V0-interval
        if ( fSpeed <= (Real)0.0 )
            return true;  // intervals moving apart

        // update first time
        fInvSpeed = ((Real)1.0)/fSpeed;
        fT = (rkCfg0.m_fMin - rkCfg1.m_fMax)*fInvSpeed;
        if ( fT > rfTFirst )
        {
            rfTFirst = fT;
            riSide = -1;
            rkTCfg0 = rkCfg0;
            rkTCfg1 = rkCfg1;
        }

        // test for exceedance of time interval
        if ( rfTFirst > fTMax )
            return true;

        // update last time
        fT = (rkCfg0.m_fMax - rkCfg1.m_fMin)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // test for separation
        if ( rfTFirst > rfTLast )
            return true;
    }
    else if ( rkCfg0.m_fMax < rkCfg1.m_fMin )
    {
        // V1-interval initially on right of V0-interval
        if ( fSpeed >= (Real)0.0 )
            return true;  // intervals moving apart

        // update first time
        fInvSpeed = ((Real)1.0)/fSpeed;
        fT = (rkCfg0.m_fMax - rkCfg1.m_fMin)*fInvSpeed;
        if ( fT > rfTFirst )
        {
            rfTFirst = fT;
            riSide = 1;
            rkTCfg0 = rkCfg0;
            rkTCfg1 = rkCfg1;
        }

        // test for exceedance of time interval
        if ( rfTFirst > fTMax )
            return true;

        // update last time
        fT = (rkCfg0.m_fMin - rkCfg1.m_fMax)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // test for separation
        if ( rfTFirst > rfTLast )
            return true;
    }
    else
    {
        // V0-interval and V1-interval initially overlap
        if ( fSpeed > (Real)0.0 )
        {
            // update last time
            fInvSpeed = ((Real)1.0)/fSpeed;
            fT = (rkCfg0.m_fMax - rkCfg1.m_fMin)*fInvSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // test for separation
            if ( rfTFirst > rfTLast )
                return true;
        }
        else if ( fSpeed < (Real)0.0 )
        {
            // update last time
            fInvSpeed = ((Real)1.0)/fSpeed;
            fT = (rkCfg0.m_fMin - rkCfg1.m_fMax)*fInvSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // test for separation
            if ( rfTFirst > rfTLast )
                return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle2Triangle2<Real>::Configuration::GetIntersection (
    const Configuration& rkCfg0, const Configuration& rkCfg1, int iSide,
    const Vector2<Real> akV0[3], const Vector2<Real> akV1[3], int& riQuantity,
    Vector2<Real> akVertex[6])
{
    Vector2<Real> kEdge, kDiff;
    const Vector2<Real>* pkOrigin;
    Real fInvEdE, fMin, fMax;
    int i;

    if ( iSide == 1 )  // V1-interval contacts V0-interval on right
    {
        if ( rkCfg0.m_eMap == M21 || rkCfg0.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV0[rkCfg0.m_aiIndex[2]];
        }
        else if ( rkCfg1.m_eMap == M12 || rkCfg1.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV1[rkCfg1.m_aiIndex[0]];
        }
        else  // rkCfg0.m_eMap == M12 && rkCfg1.m_eMap == M21 (edge overlap)
        {
            pkOrigin = &akV0[rkCfg0.m_aiIndex[1]];
            kEdge = akV0[rkCfg0.m_aiIndex[2]] - *pkOrigin;
            fInvEdE = ((Real)1.0)/kEdge.Dot(kEdge);
            kDiff = akV1[rkCfg1.m_aiIndex[1]] - *pkOrigin;
            fMin = kEdge.Dot(kDiff)*fInvEdE;
            kDiff = akV1[rkCfg1.m_aiIndex[0]] - *pkOrigin;
            fMax = kEdge.Dot(kDiff)*fInvEdE;
            assert( fMin <= fMax );
            Intersector1<Real> kIntr((Real)0.0,(Real)1.0,fMin,fMax);
            riQuantity = kIntr.GetQuantity();
            assert( riQuantity > 0 );
            for (i = 0; i < riQuantity; i++)
                akVertex[i] = *pkOrigin + kIntr.GetOverlap(i)*kEdge;
        }
    }
    else if ( iSide == -1 )  // V1-interval contacts V0-interval on left
    {
        if ( rkCfg1.m_eMap == M21 || rkCfg1.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV1[rkCfg1.m_aiIndex[2]];
        }
        else if ( rkCfg0.m_eMap == M12 || rkCfg0.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV0[rkCfg0.m_aiIndex[0]];
        }
        else  // rkCfg1.m_eMap == M12 && rkCfg0.m_eMap == M21 (edge overlap)
        {
            pkOrigin = &akV1[rkCfg1.m_aiIndex[1]];
            kEdge = akV1[rkCfg1.m_aiIndex[2]] - *pkOrigin;
            fInvEdE = 1.0f/kEdge.Dot(kEdge);
            kDiff = akV0[rkCfg0.m_aiIndex[1]] - *pkOrigin;
            fMin = kEdge.Dot(kDiff)*fInvEdE;
            kDiff = akV0[rkCfg0.m_aiIndex[0]] - *pkOrigin;
            fMax = kEdge.Dot(kDiff)*fInvEdE;
            assert( fMin <= fMax );
            Intersector1<Real> kIntr((Real)0.0,(Real)1.0,fMin,fMax);
            riQuantity = kIntr.GetQuantity();
            assert( riQuantity > 0 );
            for (i = 0; i < riQuantity; i++)
                akVertex[i] = *pkOrigin + kIntr.GetOverlap(i)*kEdge;
        }
    }
    else  // triangles were initially intersecting
    {
        Triangle2<Real> kTri0(akV0), kTri1(akV1);
        IntrTriangle2Triangle2 kIntr(kTri0,kTri1);
        kIntr.Find();
        riQuantity = kIntr.GetQuantity();
        for (i = 0; i < riQuantity; i++)
            akVertex[i] = kIntr.GetPoint(i);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrTriangle2Triangle2<float>;

// double
template WM3_ITEM
class IntrTriangle2Triangle2<double>;
//----------------------------------------------------------------------------
}
