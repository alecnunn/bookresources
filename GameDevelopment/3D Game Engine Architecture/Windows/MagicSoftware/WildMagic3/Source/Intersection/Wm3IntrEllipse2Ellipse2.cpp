// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrEllipse2Ellipse2.h"
#include "Wm3PolynomialRoots.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrEllipse2Ellipse2<Real>::IntrEllipse2Ellipse2 (
    const Ellipse2<Real>& rkEllipse0, const Ellipse2<Real>& rkEllipse1)
    :
    m_rkEllipse0(rkEllipse0),
    m_rkEllipse1(rkEllipse1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetEllipse0 () const
{
    return m_rkEllipse0;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetEllipse1 () const
{
    return m_rkEllipse1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrEllipse2Ellipse2<Real>::Test ()
{
    m_iQuantity = 0;

    Ellipse2<Real> kScaled0, kScaled1;
    Real fMin, fHalfRange;
    ScaleEllipses(kScaled0,kScaled1,fMin,fHalfRange);

    Polynomial1<Real> kPoly = GetQuartic(kScaled0,kScaled1);
    if ( kPoly.GetDegree() == -1 )
    {
        m_iIntersectionType = IT_OTHER;
        return true;
    }

    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    int iCount = kPR.GetRootCount(kPoly,-Math<Real>::MAX_REAL,
        Math<Real>::MAX_REAL);

    m_iIntersectionType = ( iCount > 0 ? IT_POINT : IT_EMPTY );
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrEllipse2Ellipse2<Real>::Find ()
{
    m_iQuantity = 0;

    Ellipse2<Real> kScaled0, kScaled1;
    Real fMin, fHalfRange;
    ScaleEllipses(kScaled0,kScaled1,fMin,fHalfRange);

    Polynomial1<Real> kPoly = GetQuartic(kScaled0,kScaled1);
    if ( kPoly.GetDegree() == -1 )
    {
        m_iIntersectionType = IT_OTHER;
        return true;
    }

    // Get the 4th-degree polynomial whose roots lead to intersections of the
    // ellipses.  The y-values are in the interval [-1,1], so we only need
    // to locate roots there.  The increase to [-1.5,1.5] is to catch roots
    // that are nearly 1 or -1 without having to worry about floating point
    // round-off errors.
    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    kPR.FindB(kPoly,-(Real)1.5,(Real)1.5,6);
    int iYCount = kPR.GetCount();
    const Real* afY = kPR.GetRoots();
    if ( iYCount == 0 )
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    // Adjustment for quadratics to allow for relative error testing when
    // eliminating extraneous roots.
    Real afQP0[6], afQP1[6];
    kScaled0.ToCoefficients(afQP0);
    kScaled1.ToCoefficients(afQP1);
    Real fNorm0 = afQP0[0]*afQP0[0] + ((Real)2.0)*afQP0[1]*afQP0[1] +
        afQP0[2]*afQP0[2];
    Real fNorm1 = afQP1[0]*afQP1[0] + ((Real)2.0)*afQP1[1]*afQP1[1] +
        afQP1[2]*afQP1[2];

    // test roots to eliminate extraneous ones that occurred due to "squaring"
    for (int iY = 0; iY < iYCount; iY++)
    {
        PolynomialRoots<Real> kAR(Math<Real>::ZERO_TOLERANCE);
        Polynomial1<Real> kAPoly(2);
        kAPoly[0] = afQP0[5]+afY[iY]*(afQP0[4]+afY[iY]*afQP0[2]);
        kAPoly[1] = afQP0[3]+afY[iY]*afQP0[1];
        kAPoly[2] = afQP0[0];
        kAR.FindB(kAPoly,6);
        int iXCount = kAR.GetCount();
        const Real* afX = kAR.GetRoots();
        for (int iX = 0; iX < iXCount; iX++)
        {
            const Real fEpsilon = (Real)1e-03;
            Real fTest = EvaluateQuadratic(afQP0,afX[iX],afY[iY]);
            if ( Math<Real>::FAbs(fTest) < fEpsilon*fNorm0 )
            {
                fTest = EvaluateQuadratic(afQP1,afX[iX],afY[iY]);
                if ( Math<Real>::FAbs(fTest) < fEpsilon*fNorm1 )
                {
                    m_akPoint[m_iQuantity].X() = afX[iX];
                    m_akPoint[m_iQuantity].Y() = afY[iY];
                    m_iQuantity++;
                }
            }
        }
    }

    if ( m_iQuantity == 0 )
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    // map intersections back to original space
    for (int i = 0; i < m_iQuantity; i++)
    {
        m_akPoint[i].X() = fHalfRange*(m_akPoint[i].X()+(Real)1.0) + fMin;
        m_akPoint[i].Y() = fHalfRange*(m_akPoint[i].Y()+(Real)1.0) + fMin;
    }

    m_iIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrEllipse2Ellipse2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrEllipse2Ellipse2<Real>::GetPoint (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetIntersectionEllipse ()
    const
{
    return m_rkEllipse0;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrEllipse2Ellipse2<Real>::GetAABB (const Ellipse2<Real>& rkEllipse,
    Real& rfXMin, Real& rfXMax, Real& rfYMin, Real& rfYMax)
{
    Real fXExtreme =
        rkEllipse.Extent[0]*Math<Real>::FAbs(rkEllipse.Axis[0].X()) +
        rkEllipse.Extent[1]*Math<Real>::FAbs(rkEllipse.Axis[1].X());
    rfXMin = rkEllipse.Center.X() - fXExtreme;
    rfXMax = rkEllipse.Center.X() + fXExtreme;

    Real fYExtreme =
        rkEllipse.Extent[0]*Math<Real>::FAbs(rkEllipse.Axis[0].Y()) +
        rkEllipse.Extent[1]*Math<Real>::FAbs(rkEllipse.Axis[1].Y());
    rfYMin = rkEllipse.Center.Y() - fYExtreme;
    rfYMax = rkEllipse.Center.Y() + fYExtreme;
}
//----------------------------------------------------------------------------
template <class Real>
Polynomial1<Real> IntrEllipse2Ellipse2<Real>::GetQuartic (
    const Ellipse2<Real>& rkEllipse0, const Ellipse2<Real>& rkEllipse1)
{
    Real afP0[6], afP1[6];
    rkEllipse0.ToCoefficients(afP0);
    rkEllipse1.ToCoefficients(afP1);

    // polynomial is constructed as a Bezout determinant
    Real afDet[11];
    afDet[ 0] = afP0[0]*afP1[1] - afP1[0]*afP0[1];  // AB
    afDet[ 1] = afP0[0]*afP1[2] - afP1[0]*afP0[2];  // AC
    afDet[ 2] = afP0[0]*afP1[3] - afP1[0]*afP0[3];  // AD
    afDet[ 3] = afP0[0]*afP1[4] - afP1[0]*afP0[4];  // AE
    afDet[ 4] = afP0[0]*afP1[5] - afP1[0]*afP0[5];  // AF
    afDet[ 5] = afP0[1]*afP1[2] - afP1[1]*afP0[2];  // BC
    afDet[ 6] = afP0[1]*afP1[4] - afP1[1]*afP0[4];  // BE
    afDet[ 7] = afP0[1]*afP1[5] - afP1[1]*afP0[5];  // BF
    afDet[ 8] = afP0[2]*afP1[3] - afP1[2]*afP0[3];  // CD
    afDet[ 9] = afP0[3]*afP1[4] - afP1[3]*afP0[4];  // DE
    afDet[10] = afP0[3]*afP1[5] - afP1[3]*afP0[5];  // DF
    Real fBFpDE = afDet[7] + afDet[9];
    Real fBEmCD = afDet[6] - afDet[8];

    Real afU[5];

    // AD*DF - AF*AF
    afU[0] = afDet[2]*afDet[10] - afDet[4]*afDet[4];

    // AB*DF + AD*(BF+DE) - 2*AE*AF
    afU[1] = afDet[0]*afDet[10] + afDet[2]*fBFpDE -
        ((Real)2.0)*afDet[3]*afDet[4];

    // AB*(BF+DE) + AD*(BE - CD) - AE*AE - 2*AC*AF
    afU[2] = afDet[0]*fBFpDE + afDet[2]*fBEmCD - afDet[3]*afDet[3] -
        ((Real)2.0)*afDet[1]*afDet[4];

    // AB*(BE - CD) + AD*BC - 2*AC*AE
    afU[3] = afDet[0]*fBEmCD + afDet[2]*afDet[5] -
        ((Real)2.0)*afDet[1]*afDet[3];

    // AB*BC - AC*AC
    afU[4] = afDet[0]*afDet[5]-afDet[1]*afDet[1];

    // reduce degree if necessary
    int iDegree = 4;
    while ( iDegree >= 0
    && Math<Real>::FAbs(afU[iDegree]) < Math<Real>::ZERO_TOLERANCE )
    {
        iDegree--;
    }

    if ( iDegree == -1 )
        return Polynomial1<Real>();

    Polynomial1<Real> kPoly(iDegree);
    memcpy(&kPoly[0],afU,(iDegree+1)*sizeof(Real));
    return kPoly;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrEllipse2Ellipse2<Real>::ScaleEllipses (Ellipse2<Real>& rkScaled0,
    Ellipse2<Real>& rkScaled1, Real& rfMin, Real& rfHalfRange)
{
    // The ellipse E[i] is contained in the oriented rectangle with vertices
    //   C[i] + s0*e0[i][0]*A0[i] + s1*e1[i][1]*A1[i]
    // where C[i] is the ellipse center, A0[i] and A1[i] are the ellipse
    // unit-length axis directions, e0[i] and e1[i] are the ellipse extents,
    // and |s0| = 1 and |s1| = 1 (four choices).  The implementation for
    // finding intersections first computes the smallest axis-aligned
    // rectangle that contains the oriented rectangles of the ellipses.  The
    // bounding rectangle is mapped to the square [-1,1]^2.  This portion of
    // the algorithm is designed to help keep the floating point calculations
    // robust in the root finding.

    // construct axis-aligned bounding rectangle for E0
    Real fXMin0, fXMax0, fYMin0, fYMax0;
    GetAABB(m_rkEllipse0,fXMin0,fXMax0,fYMin0,fYMax0);

    // construct axis-aligned bounding rectangle for E1
    Real fXMin1, fXMax1, fYMin1, fYMax1;
    GetAABB(m_rkEllipse1,fXMin1,fXMax1,fYMin1,fYMax1);

    // construct axis-aligned bounding square of bounding rectangles
    rfMin = fXMin0;
    Real fMax = fXMax0;
    if ( fXMin1 < rfMin )
        rfMin = fXMin1;

    if ( fXMax1 > fMax )
        fMax = fXMax1;

    if ( fYMin0 < rfMin )
        rfMin = fYMin0;

    if ( fYMax0 > fMax )
        fMax = fYMax0;

    if ( fYMin1 < rfMin )
        rfMin = fYMin1;

    if ( fYMax1 > fMax )
        fMax = fYMax1;

    rfHalfRange = ((Real)0.5)*(fMax - rfMin);
    Real fScale = ((Real)1.0)/rfHalfRange;

    // map ellipses to bounding square [-1,1]^2
    rkScaled0.Center.X() =
        -(Real)1.0 + fScale*(m_rkEllipse0.Center.X() - rfMin);
    rkScaled0.Center.Y() =
        -(Real)1.0 + fScale*(m_rkEllipse0.Center.Y() - rfMin);
    rkScaled1.Center.X() =
        -(Real)1.0 + fScale*(m_rkEllipse1.Center.X() - rfMin);
    rkScaled1.Center.Y() =
        -(Real)1.0 + fScale*(m_rkEllipse1.Center.Y() - rfMin);
    rkScaled0.Extent[0] = fScale*m_rkEllipse0.Extent[0];
    rkScaled0.Extent[1] = fScale*m_rkEllipse0.Extent[1];
    rkScaled1.Extent[0] = fScale*m_rkEllipse1.Extent[0];
    rkScaled1.Extent[1] = fScale*m_rkEllipse1.Extent[1];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrEllipse2Ellipse2<Real>::EvaluateQuadratic (const Real afQP[6],
    Real fX, Real fY)
{
    return (afQP[0]*fX + afQP[1]*fY + afQP[3])*fX + (afQP[2]*fY + afQP[4])*fY
        + afQP[5];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrEllipse2Ellipse2<float>;

// double
template WM3_ITEM
class IntrEllipse2Ellipse2<double>;
//----------------------------------------------------------------------------
}
