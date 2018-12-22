// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrTriangle3Triangle3.h"
#include "Wm3Intersector1.h"
#include "Wm3IntrTriangle2Triangle2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrTriangle3Triangle3<Real>::IntrTriangle3Triangle3 (
    const Triangle3<Real>& rkTriangle0, const Triangle3<Real>& rkTriangle1)
    :
    m_rkTriangle0(rkTriangle0),
    m_rkTriangle1(rkTriangle1)
{
    ReportCoplanarIntersections = true;
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrTriangle3Triangle3<Real>::GetTriangle0 () const
{
    return m_rkTriangle0;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrTriangle3Triangle3<Real>::GetTriangle1 () const
{
    return m_rkTriangle1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::Test ()
{
    // get edge vectors for triangle0
    Vector3<Real> akE0[3] =
    {
        m_rkTriangle0.V[1] - m_rkTriangle0.V[0],
        m_rkTriangle0.V[2] - m_rkTriangle0.V[1],
        m_rkTriangle0.V[0] - m_rkTriangle0.V[2]
    };

    // get normal vector of triangle0
    Vector3<Real> kN0 = akE0[0].UnitCross(akE0[1]);

    // project triangle1 onto normal line of triangle0, test for separation
    Real fN0dT0V0 = kN0.Dot(m_rkTriangle0.V[0]);
    Real fMin1, fMax1;
    ProjectOntoAxis(m_rkTriangle1,kN0,fMin1,fMax1);
    if ( fN0dT0V0 < fMin1 || fN0dT0V0 > fMax1 )
        return false;

    // get edge vectors for triangle1
    Vector3<Real> akE1[3] =
    {
        m_rkTriangle1.V[1] - m_rkTriangle1.V[0],
        m_rkTriangle1.V[2] - m_rkTriangle1.V[1],
        m_rkTriangle1.V[0] - m_rkTriangle1.V[2]
    };

    // get normal vector of triangle1
    Vector3<Real> kN1 = akE1[0].UnitCross(akE1[1]);

    Vector3<Real> kDir;
    Real fMin0, fMax0;
    int i0, i1;

    Vector3<Real> kN0xN1 = kN0.UnitCross(kN1);
    if ( kN0xN1.Dot(kN0xN1) >= Math<Real>::ZERO_TOLERANCE )
    {
        // triangles are not parallel

        // Project triangle0 onto normal line of triangle1, test for
        // separation.
        Real fN1dT1V0 = kN1.Dot(m_rkTriangle1.V[0]);
        ProjectOntoAxis(m_rkTriangle0,kN1,fMin0,fMax0);
        if ( fN1dT1V0 < fMin0 || fN1dT1V0 > fMax0 )
            return false;

        // directions E0[i0]xE1[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            for (i0 = 0; i0 < 3; i0++)
            {
                kDir = akE0[i0].UnitCross(akE1[i1]);
                ProjectOntoAxis(m_rkTriangle0,kDir,fMin0,fMax0);
                ProjectOntoAxis(m_rkTriangle1,kDir,fMin1,fMax1);
                if ( fMax0 < fMin1 || fMax1 < fMin0 )
                    return false;
            }
        }
    }
    else  // triangles are parallel (and, in fact, coplanar)
    {
        // directions N0xE0[i0]
        for (i0 = 0; i0 < 3; i0++)
        {
            kDir = kN0.UnitCross(akE0[i0]);
            ProjectOntoAxis(m_rkTriangle0,kDir,fMin0,fMax0);
            ProjectOntoAxis(m_rkTriangle1,kDir,fMin1,fMax1);
            if ( fMax0 < fMin1 || fMax1 < fMin0 )
                return false;
        }

        // directions N1xE1[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            kDir = kN1.UnitCross(akE1[i1]);
            ProjectOntoAxis(m_rkTriangle0,kDir,fMin0,fMax0);
            ProjectOntoAxis(m_rkTriangle1,kDir,fMin1,fMax1);
            if ( fMax0 < fMin1 || fMax1 < fMin0 )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::Find ()
{
    // get the plane of triangle0
    Plane3<Real> kPlane0(m_rkTriangle0.V[0],m_rkTriangle0.V[1],
        m_rkTriangle0.V[2]);

    // Compute the signed distances of triangle1 vertices to plane0.  Use
    // an epsilon-thick plane test.
    int iPos1, iNeg1, iZero1, aiSign1[3];
    Real afDist1[3];
    TrianglePlaneRelations(m_rkTriangle1,kPlane0,afDist1,aiSign1,iPos1,iNeg1,
        iZero1);

    if ( iPos1 == 3 || iNeg1 == 3 )
    {
        // triangle1 is fully on one side of plane0
        return false;
    }

    if ( iZero1 == 3 )
    {
        // triangle1 is contained by plane0
        if ( ReportCoplanarIntersections )
        {
            return GetCoplanarIntersection(kPlane0,m_rkTriangle0,
                m_rkTriangle1);
        }

        return false;
    }

    // get the plane of triangle1
    Plane3<Real> kPlane1(m_rkTriangle1.V[0],m_rkTriangle1.V[1],
        m_rkTriangle1.V[2]);

    // Compute the signed distances of triangle0 vertices to plane1.  Use
    // an epsilon-thick plane test.
    int iPos0, iNeg0, iZero0, aiSign0[3];
    Real afDist0[3];
    TrianglePlaneRelations(m_rkTriangle0,kPlane1,afDist0,aiSign0,iPos0,iNeg0,
        iZero0);

    if ( iPos0 == 3 || iNeg0 == 3 )
    {
        // triangle0 is fully on one side of plane1
        return false;
    }

    if ( iZero0 == 3 )
    {
        // triangle0 is contained by plane1
        if ( ReportCoplanarIntersections )
        {
            return GetCoplanarIntersection(kPlane1,m_rkTriangle0,
                m_rkTriangle1);
        }

        return false;
    }

    // compute the line of intersection
    Real fDot = kPlane0.Normal.Dot(kPlane1.Normal);
    Real fInvDet = ((Real)1.0)/((Real)1.0 - fDot*fDot);
    Real fC0 = (kPlane0.Constant - fDot*kPlane1.Constant)*fInvDet;
    Real fC1 = (kPlane1.Constant - fDot*kPlane0.Constant)*fInvDet;
    Line3<Real> kLine(fC0*kPlane0.Normal + fC1*kPlane1.Normal,
        kPlane0.Normal.UnitCross(kPlane1.Normal));

    // compute the intervals of intersection of triangles and line
    Real afT0[2], afT1[2];
    GetInterval(m_rkTriangle0,kLine,afDist0,aiSign0,afT0);
    GetInterval(m_rkTriangle1,kLine,afDist1,aiSign1,afT1);

    // compute the intersection of intervals
    Intersector1<Real> kIntersector(afT0,afT1);
    kIntersector.Find();
    m_iQuantity = kIntersector.GetQuantity();
    for (int i = 0; i < m_iQuantity; i++)
    {
        m_akPoint[i] = kLine.Origin +
            kIntersector.GetOverlap(i)*kLine.Direction;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::Test (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Real fTFirst = (Real)0.0;
    Real fTLast = Math<Real>::MAX_REAL;

    // velocity relative to triangle0
    Vector3<Real> kVel = rkVelocity1 - rkVelocity0;

    // compute edge and normal directions for triangle0
    Vector3<Real> akE[3] =
    {
        m_rkTriangle0.V[1] - m_rkTriangle0.V[0],
        m_rkTriangle0.V[2] - m_rkTriangle0.V[1],
        m_rkTriangle0.V[0] - m_rkTriangle0.V[2]
    };
    Vector3<Real> kN = akE[0].UnitCross(akE[1]);
    if ( !TestOverlap(kN,fTMax,kVel,fTFirst,fTLast) )
        return false;

    // compute edge and normal directions for triangle1
    Vector3<Real> akF[3] =
    {
        m_rkTriangle1.V[1] - m_rkTriangle1.V[0],
        m_rkTriangle1.V[2] - m_rkTriangle1.V[1],
        m_rkTriangle1.V[0] - m_rkTriangle1.V[2]
    };
    Vector3<Real> kM = akF[0].UnitCross(akF[1]);

    Vector3<Real> kDir;
    int i0, i1;

    if ( Math<Real>::FAbs(kN.Dot(kM)) >= 1.0f - Math<Real>::ZERO_TOLERANCE )
    {
        // triangles are not parallel

        // direction M
        if ( !TestOverlap(kM,fTMax,kVel,fTFirst,fTLast) )
            return false;

        // directions E[i0]xF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            for (i0 = 0; i0 < 3; i0++)
            {
                kDir = akE[i0].UnitCross(akF[i1]);
                if ( !TestOverlap(kDir,fTMax,kVel,fTFirst,fTLast) )
                    return false;
            }
        }
    }
    else  // triangles are parallel (and, in fact, coplanar)
    {
        // directions NxE[i0]
        for (i0 = 0; i0 < 3; i0++)
        {
            kDir = kN.UnitCross(akE[i0]);
            if ( !TestOverlap(kDir,fTMax,kVel,fTFirst,fTLast) )
                return false;
        }

        // directions NxF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            kDir = kM.UnitCross(akF[i1]);
            if ( !TestOverlap(kDir,fTMax,kVel,fTFirst,fTLast) )
                return false;
        }
    }

    m_fContactTime = fTFirst;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Real fTFirst = (Real)0.0;
    Real fTLast = Math<Real>::MAX_REAL;

    // velocity relative to triangle0
    Vector3<Real> kVel = rkVelocity1 - rkVelocity0;

    ContactSide eSide = CS_NONE;
    Configuration kTCfg0, kTCfg1;

    // compute edge and normal directions for triangle0
    Vector3<Real> akE[3] =
    {
        m_rkTriangle0.V[1] - m_rkTriangle0.V[0],
        m_rkTriangle0.V[2] - m_rkTriangle0.V[1],
        m_rkTriangle0.V[0] - m_rkTriangle0.V[2]
    };
    Vector3<Real> kN = akE[0].UnitCross(akE[1]);
    if ( !FindOverlap(kN,fTMax,kVel,eSide,kTCfg0,kTCfg1,fTFirst,fTLast) )
        return false;

    // compute edge and normal directions for triangle1
    Vector3<Real> akF[3] =
    {
        m_rkTriangle1.V[1] - m_rkTriangle1.V[0],
        m_rkTriangle1.V[2] - m_rkTriangle1.V[1],
        m_rkTriangle1.V[0] - m_rkTriangle1.V[2]
    };
    Vector3<Real> kM = akF[0].UnitCross(akF[1]);

    Vector3<Real> kDir;
    int i0, i1;

    if ( Math<Real>::FAbs(kN.Dot(kM)) >= 1.0f - Math<Real>::ZERO_TOLERANCE )
    {
        // triangles are not parallel

        // direction M
        if ( !FindOverlap(kM,fTMax,kVel,eSide,kTCfg0,kTCfg1,fTFirst,fTLast) )
            return false;

        // directions E[i0]xF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            for (i0 = 0; i0 < 3; i0++)
            {
                kDir = akE[i0].UnitCross(akF[i1]);
                if ( !FindOverlap(kDir,fTMax,kVel,eSide,kTCfg0,kTCfg1,fTFirst,
                    fTLast) )
                {
                    return false;
                }
            }
        }
    }
    else  // triangles are parallel (and, in fact, coplanar)
    {
        // directions NxE[i0]
        for (i0 = 0; i0 < 3; i0++)
        {
            kDir = kN.UnitCross(akE[i0]);
            if ( !FindOverlap(kDir,fTMax,kVel,eSide,kTCfg0,kTCfg1,fTFirst,
                fTLast) )
            {
                return false;
            }
        }

        // directions NxF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            kDir = kM.UnitCross(akF[i1]);
            if ( !FindOverlap(kDir,fTMax,kVel,eSide,kTCfg0,kTCfg1,fTFirst,
                fTLast) )
            {
                return false;
            }
        }
    }
    
    if ( fTFirst <= (Real)0.0 )
        return false;

    m_fContactTime = fTFirst;

    // adjust U and V for first time of contact before finding contact set
    Triangle3<Real> akMTri0, akMTri1;
    for (i0 = 0; i0 < 3; i0++)
    {
        akMTri0.V[i0] = m_rkTriangle0.V[i0] + fTFirst*rkVelocity0;
        akMTri1.V[i0] = m_rkTriangle1.V[i0] + fTFirst*rkVelocity1;
    }

    FindContactSet(akMTri0,akMTri1,eSide,kTCfg0,kTCfg1);
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrTriangle3Triangle3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrTriangle3Triangle3<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle3Triangle3<Real>::ProjectOntoAxis (
    const Triangle3<Real>& rkTri, const Vector3<Real>& rkAxis, Real& rfMin,
    Real& rfMax)
{
    Real fDot0 = rkAxis.Dot(rkTri.V[0]);
    Real fDot1 = rkAxis.Dot(rkTri.V[1]);
    Real fDot2 = rkAxis.Dot(rkTri.V[2]);

    rfMin = fDot0;
    rfMax = rfMin;

    if ( fDot1 < rfMin )
        rfMin = fDot1;
    else if ( fDot1 > rfMax )
        rfMax = fDot1;

    if ( fDot2 < rfMin )
        rfMin = fDot2;
    else if ( fDot2 > rfMax )
        rfMax = fDot2;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle3Triangle3<Real>::TrianglePlaneRelations (
    const Triangle3<Real>& rkTriangle, const Plane3<Real>& rkPlane,
    Real afDistance[3], int aiSign[3], int& riPositive, int& riNegative,
    int& riZero)
{
    // Compute the signed distances of triangle vertices to the plane.  Use
    // an epsilon-thick plane test.
    riPositive = 0;
    riNegative = 0;
    riZero = 0;
    for (int i = 0; i < 3; i++)
    {
        afDistance[i] = rkPlane.DistanceTo(rkTriangle.V[i]);
        if ( afDistance[i] > Math<Real>::ZERO_TOLERANCE )
        {
            aiSign[i] = 1;
            riPositive++;
        }
        else if ( afDistance[i] < -Math<Real>::ZERO_TOLERANCE )
        {
            aiSign[i] = -1;
            riNegative++;
        }
        else
        {
            afDistance[i] = (Real)0.0;
            aiSign[i] = 0;
            riZero++;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle3Triangle3<Real>::GetInterval (
    const Triangle3<Real>& rkTriangle, const Line3<Real>& rkLine,
    const Real afDistance[3], const int aiSign[3], Real afParam[2])
{
    // project triangle onto line
    Real afProj[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        Vector3<Real> kDiff = rkTriangle.V[i] - rkLine.Origin;
        afProj[i] = rkLine.Direction.Dot(kDiff);
    }

    // compute transverse intersections of triangle edges with line
    Real fNumer, fDenom;
    int i0, i1, i2;
    int iQuantity = 0;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        if ( aiSign[i0]*aiSign[i1] < 0 )
        {
            assert( iQuantity < 2 );
            fNumer = afDistance[i0]*afProj[i1] - afDistance[i1]*afProj[i0];
            fDenom = afDistance[i0] - afDistance[i1];
            afParam[iQuantity++] = fNumer/fDenom;
        }
    }

    // check for grazing contact
    if ( iQuantity < 2 )
    {
        for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2, i2++)
        {
            if ( aiSign[i2] == 0 )
            {
                assert( iQuantity < 2 );
                fNumer = afDistance[i0]*afProj[i1]-afDistance[i1]*afProj[i0];
                fDenom = afDistance[i0] - afDistance[i1];
                afParam[iQuantity++] = fNumer/fDenom;
            }
        }
    }

    // sort
    assert( iQuantity >= 1 );
    if ( iQuantity == 2 )
    {
        if ( afParam[0] > afParam[1] )
        {
            Real fSave = afParam[0];
            afParam[0] = afParam[1];
            afParam[1] = fSave;
        }
    }
    else
    {
        afParam[1] = afParam[0];
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::GetCoplanarIntersection (
    const Plane3<Real>& rkPlane, const Triangle3<Real>& rkTri0,
    const Triangle3<Real>& rkTri1)
{
    // Project triangles onto coordinate plane most aligned with plane
    // normal.
    int iMaxNormal = 0;
    Real fMax = Math<Real>::FAbs(rkPlane.Normal.X());
    Real fAbs = Math<Real>::FAbs(rkPlane.Normal.Y());
    if ( fAbs > fMax )
    {
        iMaxNormal = 1;
        fMax = fAbs;
    }
    fAbs = Math<Real>::FAbs(rkPlane.Normal.Z());
    if ( fAbs > fMax )
        iMaxNormal = 2;

    Triangle2<Real> kProjTri0, kProjTri1;
    int i;

    if ( iMaxNormal == 0 )
    {
        // project onto yz-plane
        for (i = 0; i < 3; i++)
        {
            kProjTri0.V[i].X() = m_rkTriangle0.V[i].Y();
            kProjTri0.V[i].Y() = m_rkTriangle0.V[i].Z();
            kProjTri1.V[i].X() = m_rkTriangle1.V[i].Y();
            kProjTri1.V[i].Y() = m_rkTriangle1.V[i].Z();
        }
    }
    else if ( iMaxNormal == 1 )
    {
        // project onto xz-plane
        for (i = 0; i < 3; i++)
        {
            kProjTri0.V[i].X() = m_rkTriangle0.V[i].X();
            kProjTri0.V[i].Y() = m_rkTriangle0.V[i].Z();
            kProjTri1.V[i].X() = m_rkTriangle1.V[i].X();
            kProjTri1.V[i].Y() = m_rkTriangle1.V[i].Z();
        }
    }
    else
    {
        // project onto xy-plane
        for (i = 0; i < 3; i++)
        {
            kProjTri0.V[i].X() = m_rkTriangle0.V[i].X();
            kProjTri0.V[i].Y() = m_rkTriangle0.V[i].Y();
            kProjTri1.V[i].X() = m_rkTriangle1.V[i].X();
            kProjTri1.V[i].Y() = m_rkTriangle1.V[i].Y();
        }
    }

    // 2D triangle intersection routines require counterclockwise ordering
    Vector2<Real> kSave;
    Vector2<Real> kEdge0 = kProjTri0.V[1] - kProjTri0.V[0];
    Vector2<Real> kEdge1 = kProjTri0.V[2] - kProjTri0.V[0];
    if ( kEdge0.DotPerp(kEdge1) < (Real)0.0 )
    {
        // triangle is clockwise, reorder it
        kSave = kProjTri0.V[1];
        kProjTri0.V[1] = kProjTri0.V[2];
        kProjTri0.V[2] = kSave;
    }

    kEdge0 = kProjTri1.V[1] - kProjTri1.V[0];
    kEdge1 = kProjTri1.V[2] - kProjTri1.V[0];
    if ( kEdge0.DotPerp(kEdge1) < (Real)0.0 )
    {
        // triangle is clockwise, reorder it
        kSave = kProjTri1.V[1];
        kProjTri1.V[1] = kProjTri1.V[2];
        kProjTri1.V[2] = kSave;
    }

    IntrTriangle2Triangle2<Real> kIntr(kProjTri0,kProjTri1);
    if ( !kIntr.Find() )
        return false;

    // map 2D intersections back to the 3D triangle space
    m_iQuantity = kIntr.GetQuantity();
    if ( iMaxNormal == 0 )
    {
        Real fInvNX = ((Real)1.0)/rkPlane.Normal.X();
        for (i = 0; i < m_iQuantity; i++)
        {
            m_akPoint[i].Y() = kIntr.GetPoint(i).X();
            m_akPoint[i].Z() = kIntr.GetPoint(i).Y();
            m_akPoint[i].X() = fInvNX*(rkPlane.Constant -
                rkPlane.Normal.Y()*m_akPoint[i].Y() -
                rkPlane.Normal.Z()*m_akPoint[i].Z());
        }
    }
    else if ( iMaxNormal == 1 )
    {
        Real fInvNY = ((Real)1.0)/rkPlane.Normal.Y();
        for (i = 0; i < m_iQuantity; i++)
        {
            m_akPoint[i].X() = kIntr.GetPoint(i).X();
            m_akPoint[i].Z() = kIntr.GetPoint(i).Y();
            m_akPoint[i].Y() = fInvNY*(rkPlane.Constant -
                rkPlane.Normal.X()*m_akPoint[i].X() -
                rkPlane.Normal.Z()*m_akPoint[i].Z());
        }
    }
    else
    {
        Real fInvNZ = ((Real)1.0)/rkPlane.Normal.Z();
        for (i = 0; i < m_iQuantity; i++)
        {
            m_akPoint[i].X() = kIntr.GetPoint(i).X();
            m_akPoint[i].Y() = kIntr.GetPoint(i).Y();
            m_akPoint[i].Z() = fInvNZ*(rkPlane.Constant -
                rkPlane.Normal.X()*m_akPoint[i].X() -
                rkPlane.Normal.Y()*m_akPoint[i].Y());
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::TestOverlap (const Vector3<Real>& rkAxis,
    Real fTMax, Real fSpeed, Real fUMin, Real fUMax, Real fVMin, Real fVMax,
    Real& rfTFirst, Real& rfTLast)
{
    // Constant velocity separating axis test.

    Real fT;

    if ( fVMax < fUMin ) // V on left of U
    {
        if ( fSpeed <= (Real)0.0 ) // V moving away from U
            return false;

        // find first time of contact on this axis
        fT = (fUMin - fVMax)/fSpeed;
        if ( fT > rfTFirst )
            rfTFirst = fT;

        // quick out: intersection after desired time interval
        if ( rfTFirst > fTMax )
            return false;   

        // find last time of contact on this axis
        fT = (fUMax - fVMin)/fSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // quick out: intersection before desired time interval
        if ( rfTFirst > rfTLast )
            return false; 
    }
    else if ( fUMax < fVMin )   // V on right of U
    {
        if ( fSpeed >= (Real)0.0 ) // V moving away from U
            return false;

        // find first time of contact on this axis
        fT = (fUMax - fVMin)/fSpeed;
        if ( fT > rfTFirst )
            rfTFirst = fT;

        // quick out: intersection after desired time interval
        if ( rfTFirst > fTMax )
            return false;   

        // find last time of contact on this axis
        fT = (fUMin - fVMax)/fSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // quick out: intersection before desired time interval
        if ( rfTFirst > rfTLast )
            return false; 

    }
    else // V and U on overlapping interval
    {
        if ( fSpeed > (Real)0.0 )
        {
            // find last time of contact on this axis
            fT = (fUMax - fVMin)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // quick out: intersection before desired interval
            if ( rfTFirst > rfTLast )
                return false; 
        }
        else if ( fSpeed < (Real)0.0 )
        {
            // find last time of contact on this axis
            fT = (fUMin - fVMax)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // quick out: intersection before desired interval
            if ( rfTFirst > rfTLast )
                return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::FindOverlap (const Vector3<Real>& rkAxis,
    Real fTMax, Real fSpeed, const Configuration& rkUC, 
    const Configuration& rkVC, ContactSide& rkSide, Configuration& rkTUC,
    Configuration& rkTVC, Real& rfTFirst, Real& rfTLast)
{
    // Constant velocity separating axis test.  UC and VC are the new
    // potential configurations, and TUC and TVC are the best known
    // configurations.

    Real fT;

    if ( rkVC.Max < rkUC.Min ) // V on left of U
    {
        if ( fSpeed <= (Real)0.0 ) // V moving away from U
            return false;

        // find first time of contact on this axis
        fT = (rkUC.Min - rkVC.Max)/fSpeed;

        // If this is the new maximum first time of contact, set side and
        // configuration.
        if ( fT > rfTFirst )
        {
            rfTFirst = fT;
            rkSide = CS_LEFT;
            rkTUC = rkUC;
            rkTVC = rkVC;
        }

        // quick out: intersection after desired interval
        if ( rfTFirst > fTMax )
            return false;

        // find last time of contact on this axis
        fT = (rkUC.Max - rkVC.Min)/fSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // quick out: intersection before desired interval
        if ( rfTFirst > rfTLast )
            return false;
    }
    else if ( rkUC.Max < rkVC.Min )   // V on right of U
    {
        if ( fSpeed >= (Real)0.0 ) // V moving away from U
            return false;

        // find first time of contact on this axis
        fT = (rkUC.Max - rkVC.Min)/fSpeed;

        // If this is the new maximum first time of contact, set side and
        // configuration.
        if ( fT > rfTFirst )
        {
            rfTFirst = fT;
            rkSide = CS_RIGHT;
            rkTUC = rkUC;
            rkTVC = rkVC;
        }

        // quick out: intersection after desired interval
        if ( rfTFirst > fTMax )
            return false;   

        // find last time of contact on this axis
        fT = (rkUC.Min - rkVC.Max)/fSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // quick out: intersection before desired interval
        if ( rfTFirst > rfTLast )
            return false;
    }
    else // V and U on overlapping interval
    {
        if ( fSpeed > (Real)0.0 )
        {
            // find last time of contact on this axis
            fT = (rkUC.Max - rkVC.Min)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // quick out: intersection before desired interval
            if ( rfTFirst > rfTLast )
                return false; 
        }
        else if ( fSpeed < (Real)0.0 )
        {
            // find last time of contact on this axis
            fT = (rkUC.Min - rkVC.Max)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // quick out: intersection before desired interval
            if ( rfTFirst > rfTLast )
                return false; 
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::TestOverlap (const Vector3<Real>& rkAxis,
    Real fTMax, const Vector3<Real>& rkVelocity, Real& rfTFirst,
    Real& rfTLast)
{
    Real fMin0, fMax0, fMin1, fMax1;
    ProjectOntoAxis(m_rkTriangle0,rkAxis,fMin0,fMax0);
    ProjectOntoAxis(m_rkTriangle1,rkAxis,fMin1,fMax1);
    Real fSpeed = rkVelocity.Dot(rkAxis);
    return TestOverlap(rkAxis,fTMax,fSpeed,fMin0,fMax0,fMin1,fMax1,rfTFirst,
        rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Triangle3<Real>::FindOverlap (const Vector3<Real>& rkAxis,
    Real fTMax, const Vector3<Real>& rkVelocity, ContactSide& reSide,
    Configuration& rkTCfg0, Configuration& rkTCfg1, Real& rfTFirst,
    Real& rfTLast)
{
    Configuration kCfg0, kCfg1;
    ProjectOntoAxis(m_rkTriangle0,rkAxis,kCfg0);
    ProjectOntoAxis(m_rkTriangle1,rkAxis,kCfg1);
    Real fSpeed = rkVelocity.Dot(rkAxis);
    return FindOverlap(rkAxis,fTMax,fSpeed,kCfg0,kCfg1,reSide,rkTCfg0,rkTCfg1,
        rfTFirst,rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle3Triangle3<Real>::ProjectOntoAxis (
    const Triangle3<Real>& rkTri, const Vector3<Real>& rkAxis,
    Configuration& rkCfg)
{
    // find projections of vertices onto potential separating axis
    Real fD0 = rkAxis.Dot(rkTri.V[0]);
    Real fD1 = rkAxis.Dot(rkTri.V[1]);
    Real fD2 = rkAxis.Dot(rkTri.V[2]);

    // explicit sort of vertices to construct a Configuration object
    if ( fD0 <= fD1 )
    {
        if ( fD1 <= fD2 ) // D0 <= D1 <= D2
        {
            if ( fD0 != fD1 )
            {
                if ( fD1 != fD2 )
                    rkCfg.Map = M111;
                else
                    rkCfg.Map = M12;
            }
            else // ( D0 == D1 )
            {
                if ( fD1 != fD2 )
                    rkCfg.Map = M21;
                else
                    rkCfg.Map = M3;
            }
            rkCfg.Index[0] = 0;
            rkCfg.Index[1] = 1;
            rkCfg.Index[2] = 2;
            rkCfg.Min = fD0;
            rkCfg.Max = fD2;
        }
        else if ( fD0 <= fD2 ) // D0 <= D2 < D1
        {
            if ( fD0 != fD2 )
            {
                rkCfg.Map = M111;
                rkCfg.Index[0] = 0;
                rkCfg.Index[1] = 2;
                rkCfg.Index[2] = 1;
            }
            else
            {
                rkCfg.Map = M21;
                rkCfg.Index[0] = 2;
                rkCfg.Index[1] = 0;
                rkCfg.Index[2] = 1;
            }
            rkCfg.Min = fD0;
            rkCfg.Max = fD1;
        }
        else // D2 < D0 <= D1
        {
            if ( fD0 != fD1 )
                rkCfg.Map = M111;
            else
                rkCfg.Map = M12;

            rkCfg.Index[0] = 2;
            rkCfg.Index[1] = 0;
            rkCfg.Index[2] = 1;
            rkCfg.Min = fD2;
            rkCfg.Max = fD1;
        }
    }
    else if ( fD2 <= fD1 ) // D2 <= D1 < D0
    {
        if ( fD2 != fD1 )
        {
            rkCfg.Map = M111;
            rkCfg.Index[0] = 2;
            rkCfg.Index[1] = 1;
            rkCfg.Index[2] = 0;
        }
        else
        {
            rkCfg.Map = M21;
            rkCfg.Index[0] = 1;
            rkCfg.Index[1] = 2;
            rkCfg.Index[2] = 0;

        }
        rkCfg.Min = fD2;
        rkCfg.Max = fD0;
    }
    else if ( fD2 <= fD0 ) // D1 < D2 <= D0
    {
        if ( fD2 != fD0 ) 
            rkCfg.Map = M111;
        else
            rkCfg.Map = M12;

        rkCfg.Index[0] = 1;
        rkCfg.Index[1] = 2;
        rkCfg.Index[2] = 0;
        rkCfg.Min = fD1;
        rkCfg.Max = fD0;
    }
    else // D1 < D0 < D2
    {
        rkCfg.Map = M111;
        rkCfg.Index[0] = 1;
        rkCfg.Index[1] = 0;
        rkCfg.Index[2] = 2;
        rkCfg.Min = fD1;
        rkCfg.Max = fD2;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle3Triangle3<Real>::FindContactSet (
    const Triangle3<Real>& rkTri0, const Triangle3<Real>& rkTri1,
    ContactSide& reSide, Configuration& rkCfg0, Configuration& rkCfg1)
{
    if ( reSide == CS_RIGHT ) // tri1 to the right of tri0
    {
        if ( rkCfg0.Map == M21 || rkCfg0.Map == M111 )
        {
            // tri0 touching tri1 at a single point
            m_iQuantity = 1;
            m_akPoint[0] = rkTri0.V[2];
        }
        else if ( rkCfg1.Map == M12 || rkCfg1.Map == M111 )
        {
            // tri1 touching tri0 at a single point
            m_iQuantity = 1;
            m_akPoint[0] = rkTri1.V[0];
        }
        else if ( rkCfg0.Map == M12 )
        {
            if ( rkCfg1.Map == M21 )
            {
                // edge0-edge1 intersection
                GetEdgeEdgeIntersection(rkTri0.V[1],rkTri0.V[2],rkTri1.V[0],
                    rkTri1.V[1]);
            }
            else // ( rkCfg1.Map == m3 )
            {
                // uedge-vface intersection
                Vector3<Real> akEdge0[2] = { rkTri0.V[1], rkTri0.V[2] };
                //FindContactSetColinearLineTri(akEdge0,rkTri1,riQuantity,akP);
            }
        }
        else // ( rkCfg0.Map == M3 )
        {
            if ( rkCfg1.Map == M21 )
            {

                // face0-edge1 intersection
                Vector3<Real> akEdge1[2] = { rkTri1.V[0], rkTri1.V[1] };
                //FindContactSetColinearLineTri(akEdge1,rkTri0,riQuantity,akP);
            } 
            else // ( rkCfg1.Map == M3 )
            {
                // face0-face1 intersection
                Plane3<Real> kPlane0(rkTri0.V[0],rkTri0.V[1],rkTri0.V[2]);
                GetCoplanarIntersection(kPlane0,rkTri0,rkTri1);
            }
        }
    }
    else if ( reSide == CS_LEFT ) // tri1 to the left of tri0
    {
        if ( rkCfg1.Map == M21 || rkCfg1.Map == M111 )
        {
            // tri1 touching tri0 at a single point
            m_iQuantity = 1;
            m_akPoint[0] = rkTri1.V[2];
        }
        else if ( rkCfg0.Map == M12 || rkCfg0.Map == M111 )
        {
            // tri0 touching tri1 at a single point
            m_iQuantity = 1;
            m_akPoint[0] = rkTri0.V[0];
        }
        else if ( rkCfg1.Map == M12 )
        {
            if ( rkCfg0.Map == M21 )
            {
                // edge0-edge1 intersection
                GetEdgeEdgeIntersection(rkTri0.V[0],rkTri0.V[1],rkTri1.V[1],
                    rkTri1.V[2]);
            }
            else // ( rkCfg0.Map == M3 )
            {
                // edge1-face0 intersection
                Vector3<Real> akEdge1[2] = { rkTri1.V[1], rkTri1.V[2] };
                //FindContactSetColinearLineTri(akEdge1,rkTri0,riQuantity,akP);
            }
        }
        else // ( rkCfg1.Map == M3 )
        {
            if ( rkCfg0.Map == M21 )
            {
                // edge0-face1 intersection
                Vector3<Real> akEdge0[2] = { rkTri0.V[0], rkTri0.V[1] };
                //FindContactSetColinearLineTri(akEdge0,rkTri1,riQuantity,akP);
            } 
            else // ( rkCfg0.Map == M3 )
            {
                // face0-face1 intersection
                Plane3<Real> kPlane0(rkTri0.V[0],rkTri0.V[1],rkTri0.V[2]);
                GetCoplanarIntersection(kPlane0,rkTri0,rkTri1);
            }
        }
    }
    else // ( reSide == CS_NONE )
    {
        // triangles are already intersecting tranversely
        IntrTriangle3Triangle3<Real>(rkTri0,rkTri1).Find();
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrTriangle3Triangle3<Real>::GetEdgeEdgeIntersection (
    const Vector3<Real>& rkU0, const Vector3<Real>& rkU1,
    const Vector3<Real>& rkV0, const Vector3<Real>& rkV1)
{
    // TO DO.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrTriangle3Triangle3<float>;

// double
template WM3_ITEM
class IntrTriangle3Triangle3<double>;
//----------------------------------------------------------------------------
}
