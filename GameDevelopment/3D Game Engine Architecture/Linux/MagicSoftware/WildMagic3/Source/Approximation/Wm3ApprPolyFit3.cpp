// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprPolyFit3.h"
#include "Wm3LinearSystem.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Real* PolyFit3 (int iSamples, const Real* afX, const Real* afY,
    const Real* afW, int iXDegree, int iYDegree)
{
    int iXBound = iXDegree + 1;
    int iYBound = iYDegree + 1;
    int iQuantity = iXBound*iYBound;
    Real* afCoeff = new Real[iQuantity];

    int i0, j0, i1, j1, iS;

    // powers of x, y
    Real** aafXP;
    Real** aafYP;
    System::Allocate(2*iXBound,iSamples,aafXP);
    System::Allocate(2*iYBound,iSamples,aafYP);
    for (iS = 0; iS < iSamples; iS++)
    {
        aafXP[iS][0] = (Real)1.0;
        for (i0 = 1; i0 <= 2*iXDegree; i0++)
            aafXP[iS][i0] = afX[iS]*aafXP[iS][i0-1];

        aafYP[iS][0] = (Real)1.0;
        for (j0 = 1; j0 <= 2*iYDegree; j0++)
            aafYP[iS][j0] = afY[iS]*aafYP[iS][j0-1];
    }

    // Vandermonde matrix and right-hand side of linear system
    GMatrix<Real> kA(iQuantity,iQuantity);
    Real* afB = new Real[iQuantity];

    for (j0 = 0; j0 <= iYDegree; j0++)
    {
        for (i0 = 0; i0 <= iXDegree; i0++)
        {
            int iIndex0 = i0+iXBound*j0;
            Real fSum = (Real)0.0;
            for (iS = 0; iS < iSamples; iS++)
                fSum += afW[iS]*aafXP[iS][i0]*aafYP[iS][j0];

            afB[iIndex0] = fSum;

            for (j1 = 0; j1 <= iYDegree; j1++)
            {
                for (i1 = 0; i1 <= iXDegree; i1++)
                {
                    int iIndex1 = i1+iXBound*j1;
                    fSum = (Real)0.0;
                    for (iS = 0; iS < iSamples; iS++)
                        fSum += aafXP[iS][i0+i1]*aafYP[iS][j0+j1];

                    kA(iIndex0,iIndex1) = fSum;
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

    return afCoeff;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
float* PolyFit3<float> (int, const float*, const float*, const float*,
    int, int);

// double
template WM3_ITEM
double* PolyFit3<double> (int, const double*, const double*, const double*,
    int, int);
//----------------------------------------------------------------------------
}
