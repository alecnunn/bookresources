// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprPolyFit2.h"
#include "Wm3LinearSystem.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Real* PolyFit2 (int iSamples, const Real* afX, const Real* afW, int iXDegree)
{
    int iQuantity = iXDegree + 1;
    Real* afCoeff = new Real[iQuantity];

    int i0, i1, iS;

    // powers of x
    Real** aafXP;
    System::Allocate(2*iQuantity,iSamples,aafXP);
    for (iS = 0; iS < iSamples; iS++)
    {
        aafXP[iS][0] = (Real)1.0;
        for (i0 = 1; i0 <= 2*iXDegree; i0++)
            aafXP[iS][i0] = afX[iS]*aafXP[iS][i0-1];
    }

    // Vandermonde matrix and right-hand side of linear system
    GMatrix<Real> kA(iQuantity,iQuantity);
    Real* afB = new Real[iQuantity];

    for (i0 = 0; i0 <= iXDegree; i0++)
    {
        Real fSum = (Real)0.0;
        for (iS = 0; iS < iSamples; iS++)
            fSum += afW[iS]*aafXP[iS][i0];

        afB[i0] = fSum;

        for (i1 = 0; i1 <= iXDegree; i1++)
        {
            fSum = (Real)0.0;
            for (iS = 0; iS < iSamples; iS++)
                fSum += aafXP[iS][i0+i1];

            kA(i0,i1) = fSum;
        }
    }

    // solve for the polynomial coefficients
    bool bHasSolution = LinearSystem<Real>().Solve(kA,afB,afCoeff);
    assert( bHasSolution );
    (void)bHasSolution;  // avoid compiler warning in release build
    delete[] afB;
    System::Deallocate(aafXP);

    return afCoeff;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
float* PolyFit2<float> (int, const float*, const float*, int);

// double
template WM3_ITEM
double* PolyFit2<double> (int, const double*, const double*, int);
//----------------------------------------------------------------------------
}
