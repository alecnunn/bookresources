// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3OdeImplicitEuler.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
OdeImplicitEuler<Real>::OdeImplicitEuler (int iDim, Real fStep,
    typename OdeSolver<Real>::Function oFunction,
    DerivativeFunction oDFunction, void* pvData)
    :
    OdeSolver<Real>(iDim,fStep,oFunction,pvData),
    m_kDF(iDim,iDim),
    m_kF(iDim),
    m_kIdentity(iDim,iDim)
{
    m_oDFunction = oDFunction;
    for (int i = 0; i < iDim; i++)
        m_kIdentity[i][i] = (Real)1.0;
}
//----------------------------------------------------------------------------
template <class Real>
OdeImplicitEuler<Real>::~OdeImplicitEuler ()
{
}
//----------------------------------------------------------------------------
template <class Real>
void OdeImplicitEuler<Real>::Update (Real fTIn, Real* afXIn, Real& rfTOut,
    Real* afXOut)
{
    m_oFunction(fTIn,afXIn,m_pvData,m_kF);
    m_oDFunction(fTIn,afXIn,m_pvData,m_kDF);
    GMatrix<Real> kDG = m_kIdentity - m_fStep*m_kDF;
    GMatrix<Real> kDGInverse(m_iDim,m_iDim);
    bool bInvertible = kDG.GetInverse(kDGInverse);

    if ( bInvertible )
    {
        m_kF = kDGInverse*m_kF;
        for (int i = 0; i < m_iDim; i++)
            afXOut[i] = afXIn[i] + m_fStep*m_kF[i];
    }
    else
    {
        memcpy(afXOut,afXIn,m_iDim*sizeof(Real));
    }

    rfTOut = fTIn + m_fStep;
}
//----------------------------------------------------------------------------
template <class Real>
void OdeImplicitEuler<Real>::SetStepSize (Real fStep)
{
    m_fStep = fStep;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class OdeImplicitEuler<float>;

// double
template WM3_ITEM
class OdeImplicitEuler<double>;
//----------------------------------------------------------------------------
}
