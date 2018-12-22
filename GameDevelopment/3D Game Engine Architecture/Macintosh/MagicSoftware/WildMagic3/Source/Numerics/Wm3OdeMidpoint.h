// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3ODEMIDPOINT_H
#define WM3ODEMIDPOINT_H

#include "Wm3OdeSolver.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM OdeMidpoint : public OdeSolver<Real>
{
public:
    OdeMidpoint (int iDim, Real fStep,
        typename OdeSolver<Real>::Function oFunction, void* pvData = 0);

    virtual ~OdeMidpoint ();

    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    virtual void SetStepSize (Real fStep);

protected:
    Real m_fHalfStep;
    Real* m_afXTemp;
};

typedef OdeMidpoint<float> OdeMidpointf;
typedef OdeMidpoint<double> OdeMidpointd;

}

#endif
