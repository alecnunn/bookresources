// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3ODEIMPLICITEULER_H
#define WM3ODEIMPLICITEULER_H

#include "Wm3OdeSolver.h"
#include "Wm3GMatrix.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM OdeImplicitEuler : public OdeSolver<Real>
{
public:
    // The function F(t,x) has input t, a scalar, and input x, an n-vector.
    // The first derivative matrix with respect to x is DF(t,x), an
    // n-by-n matrix.  Entry DF[r][c] is the derivative of F[r] with
    // respect to x[c].
    typedef void (*DerivativeFunction)(
        Real,             // t
        const Real*,      // x
        void*,            // user-specified data
        GMatrix<Real>&);  // DF(t,x)

    OdeImplicitEuler (int iDim, Real fStep,
        typename OdeSolver<Real>::Function oFunction,
        DerivativeFunction oDFunction, void* pvData = 0);

    virtual ~OdeImplicitEuler ();

    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    virtual void SetStepSize (Real fStep);

protected:
    DerivativeFunction m_oDFunction;
    GMatrix<Real> m_kDF;
    GVector<Real> m_kF;
    GMatrix<Real> m_kIdentity;
};

typedef OdeImplicitEuler<float> OdeImplicitEulerf;
typedef OdeImplicitEuler<double> OdeImplicitEulerd;

}

#endif
