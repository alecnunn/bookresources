// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprPolyFit4.h"
#include "Wm3LinearSystem.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Real* PolyFit4 (int iSamples, const Real* afX, const Real* afY,
    const Real* afZ, const Real* afW, int iXDegree, int iYDegree,
    int iZDegree)
{
    int iXBound = iXDegree + 1;
    int iYBound = iYDegree + 1;
    int iZBound = iZDegree + 1;
    int iQuantity = iXBound*iYBound*iZBound;
    Real* afCoeff = new Real[iQuantity];

    int i0, j0, k0, i1, j1, k1, iS;

    // powers of x, y, z
    Real** aafXP;
    Real** aafYP;
    Real** aafZP;
    System::Allocate(2*iXBound,iSamples,aafXP);
    System::Allocate(2*iYBound,iSamples,aafYP);
    System::Allocate(2*iZBound,iSamples,aafZP);
    for (iS = 0; iS < iSamples; iS++)
    {
        aafXP[iS][0] = (Real)1.0;
        for (i0 = 1; i0 <= 2*iXDegree; i0++)
            aafXP[iS][i0] = afX[iS]*aafXP[iS][i0-1];

        aafYP[iS][0] = (Real)1.0;
        for (j0 = 1; j0 <= 2*iYDegree; j0++)
            aafYP[iS][j0] = afY[iS]*aafYP[iS][j0-1];

        aafZP[iS][0] = (Real)1.0;
        for (k0 = 1; k0 <= 2*iZDegree; k0++)
            aafZP[iS][k0] = afZ[iS]*aafZP[iS][k0-1];
    }

    // Vandermonde matrix and right-hand side of linear system
    GMatrix<Real> kA(iQuantity,iQuantity);
    Real* afB = new Real[iQuantity];

    for (k0 = 0; k0 <= iZDegree; k0++)
    {
        for (j0 = 0; j0 <= iYDegree; j0++)
        {
            for (i0 = 0; i0 <= iXDegree; i0++)
            {
                int iIndex0 = i0+iXBound*(j0+iYBound*k0);
                Real fSum = (Real)0.0;
                for (iS = 0; iS < iSamples; iS++)
                {
                    fSum += afW[iS] * aafXP[iS][i0] * aafYP[iS][j0] *
                        aafZP[iS][k0];
                }
                afB[iIndex0] = fSum;

                for (k1 = 0; k1 <= iZDegree; k1++)
                {
                    for (j1 = 0; j1 <= iYDegree; j1++)
                    {
                        for (i1 = 0; i1 <= iXDegree; i1++)
                        {
                            int iIndex1 = i1+iXBound*(j1+iYBound*k1);
                            fSum = (Real)0.0;
                            for (iS = 0; iS < iSamples; iS++)
                            {
                                fSum += aafXP[iS][i0+i1] * aafYP[iS][j0+j1] *
                                    aafZP[iS][k0+k1];
                            }
                            kA(iIndex0,iIndex1) = fSum;
                        }
                    }
                }
            }
        }
    }

    // solve for the polynomial coefficients
    bool bHasSolution = LinearSystem<Real>().Solve(kA,afB,afCoeff);
    assert( bHasSolution );
    (void)bHasSolution;  // avoid compiler warning in release build
    delete[] afB;
    System::Deallocate(aafXP);
    System::Deallocate(aafYP);
    System::Deallocate(aafZP);

    return afCoeff;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
float* PolyFit4<float> (int, const float*, const float*, const float*,
    const float*, int, int, int);

// double
template WM3_ITEM
double* PolyFit4<double> (int, const double*, const double*, const double*,
    const double*, int, int, int);
//----------------------------------------------------------------------------
}
