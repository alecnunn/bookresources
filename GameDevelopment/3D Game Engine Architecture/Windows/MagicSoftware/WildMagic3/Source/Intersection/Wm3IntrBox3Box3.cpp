// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrBox3Box3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrBox3Box3<Real>::IntrBox3Box3 (const Box3<Real>& rkBox0,
    const Box3<Real>& rkBox1)
    :
    m_rkBox0(rkBox0),
    m_rkBox1(rkBox1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrBox3Box3<Real>::GetBox0 () const
{
    return m_rkBox0;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrBox3Box3<Real>::GetBox1 () const
{
    return m_rkBox1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox3Box3<Real>::Test ()
{
    // Cutoff for cosine of angles between box axes.  This is used to catch
    // the cases when at least one pair of axes are parallel.  If this happens,
    // there is no need to test for separation along the Cross(A[i],B[j])
    // directions.
    const Real fCutoff = (Real)1.0 - Math<Real>::ZERO_TOLERANCE;
    bool bExistsParallelPair = false;
    int i;

    // convenience variables
    const Vector3<Real>* akA = m_rkBox0.Axis;
    const Vector3<Real>* akB = m_rkBox1.Axis;
    const Real* afEA = m_rkBox0.Extent;
    const Real* afEB = m_rkBox1.Extent;

    // compute difference of box centers, D = C1-C0
    Vector3<Real> kD = m_rkBox1.Center - m_rkBox0.Center;

    Real aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    Real aafAbsC[3][3];  // |c_{ij}|
    Real afAD[3];        // Dot(A_i,D)
    Real fR0, fR1, fR;   // interval radii and distance between centers
    Real fR01;           // = R0 + R1

    // axis C0+t*A0
    for (i = 0; i < 3; i++)
    {
        aafC[0][i] = akA[0].Dot(akB[i]);
        aafAbsC[0][i] = Math<Real>::FAbs(aafC[0][i]);
        if ( aafAbsC[0][i] > fCutoff )
            bExistsParallelPair = true;
    }
    afAD[0] = akA[0].Dot(kD);
    fR = Math<Real>::FAbs(afAD[0]);
    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
    fR01 = afEA[0] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1
    for (i = 0; i < 3; i++)
    {
        aafC[1][i] = akA[1].Dot(akB[i]);
        aafAbsC[1][i] = Math<Real>::FAbs(aafC[1][i]);
        if ( aafAbsC[1][i] > fCutoff )
            bExistsParallelPair = true;
    }
    afAD[1] = akA[1].Dot(kD);
    fR = Math<Real>::FAbs(afAD[1]);
    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
    fR01 = afEA[1] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2
    for (i = 0; i < 3; i++)
    {
        aafC[2][i] = akA[2].Dot(akB[i]);
        aafAbsC[2][i] = Math<Real>::FAbs(aafC[2][i]);
        if ( aafAbsC[2][i] > fCutoff )
            bExistsParallelPair = true;
    }
    afAD[2] = akA[2].Dot(kD);
    fR = Math<Real>::FAbs(afAD[2]);
    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
    fR01 = afEA[2] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*B0
    fR = Math<Real>::FAbs(akB[0].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
    fR01 = fR0 + afEB[0];
    if ( fR > fR01 )
        return false;

    // axis C0+t*B1
    fR = Math<Real>::FAbs(akB[1].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
    fR01 = fR0 + afEB[1];
    if ( fR > fR01 )
        return false;

    // axis C0+t*B2
    fR = Math<Real>::FAbs(akB[2].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
    fR01 = fR0 + afEB[2];
    if ( fR > fR01 )
        return false;

    // At least one pair of box axes was parallel, so the separation is
    // effectively in 2D where checking the "edge" normals is sufficient for
    // the separation of the boxes.
    if ( bExistsParallelPair )
        return true;

    // axis C0+t*A0xB0
    fR = Math<Real>::FAbs(afAD[2]*aafC[1][0]-afAD[1]*aafC[2][0]);
    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB1
    fR = Math<Real>::FAbs(afAD[2]*aafC[1][1]-afAD[1]*aafC[2][1]);
    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB2
    fR = Math<Real>::FAbs(afAD[2]*aafC[1][2]-afAD[1]*aafC[2][2]);
    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB0
    fR = Math<Real>::FAbs(afAD[0]*aafC[2][0]-afAD[2]*aafC[0][0]);
    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB1
    fR = Math<Real>::FAbs(afAD[0]*aafC[2][1]-afAD[2]*aafC[0][1]);
    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB2
    fR = Math<Real>::FAbs(afAD[0]*aafC[2][2]-afAD[2]*aafC[0][2]);
    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB0
    fR = Math<Real>::FAbs(afAD[1]*aafC[0][0]-afAD[0]*aafC[1][0]);
    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB1
    fR = Math<Real>::FAbs(afAD[1]*aafC[0][1]-afAD[0]*aafC[1][1]);
    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB2
    fR = Math<Real>::FAbs(afAD[1]*aafC[0][2]-afAD[0]*aafC[1][2]);
    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrBox3Box3<float>;

// double
template WM3_ITEM
class IntrBox3Box3<double>;
//----------------------------------------------------------------------------
}
