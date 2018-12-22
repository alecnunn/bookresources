// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3OdeSolver.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
OdeSolver<Real>::OdeSolver (int iDim, Real fStep, Function oFunction,
    void* pvData)
{
    m_iDim = iDim;
    m_fStep = fStep;
    m_oFunction = oFunction;
    m_pvData = pvData;
    m_afFValue = new Real[iDim];
}
//----------------------------------------------------------------------------
template <class Real>
OdeSolver<Real>::~OdeSolver ()
{
    delete[] m_afFValue;
}
//----------------------------------------------------------------------------
template <class Real>
Real OdeSolver<Real>::GetStepSize () const
{
    return m_fStep;
}
//----------------------------------------------------------------------------
template <class Real>
void OdeSolver<Real>::SetData (void* pvData)
{
    m_pvData = pvData;
}
//----------------------------------------------------------------------------
template <class Real>
void* OdeSolver<Real>::GetData () const
{
    return m_pvData;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class OdeSolver<float>;

// double
template WM3_ITEM
class OdeSolver<double>;
//----------------------------------------------------------------------------
}
