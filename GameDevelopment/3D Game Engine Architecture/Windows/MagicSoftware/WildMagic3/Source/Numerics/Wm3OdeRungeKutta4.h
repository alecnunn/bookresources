// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RUNGEKUTTA4_H
#define WM3RUNGEKUTTA4_H

#include "Wm3OdeSolver.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM OdeRungeKutta4 : public OdeSolver<Real>
{
public:
    OdeRungeKutta4 (int iDim, Real fStep,
        typename OdeSolver<Real>::Function oFunction, void* pvData = 0);

    virtual ~OdeRungeKutta4 ();

    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    virtual void SetStepSize (Real fStep);

protected:
    Real m_fHalfStep, m_fSixthStep;
    Real* m_afTemp1;
    Real* m_afTemp2;
    Real* m_afTemp3;
    Real* m_afTemp4;
    Real* m_afXTemp;
};

typedef OdeRungeKutta4<float> OdeRungeKutta4f;
typedef OdeRungeKutta4<double> OdeRungeKutta4d;

}

#endif
