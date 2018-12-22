// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntpVectorField2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntpVectorField2<Real>::IntpVectorField2 (int iQuantity,
    Vector2<Real>* akInput, Vector2<Real>* akOutput, bool bOwner)
{
    // Repackage the output vectors into individual components.  This is
    // required because of the format that the quadratic interpolator expects
    // for its input data.
    Real* afXOutput = new Real[iQuantity];
    Real* afYOutput = new Real[iQuantity];
    for (int i = 0; i < iQuantity; i++)
    {
        afXOutput[i] = akOutput[i].X();
        afYOutput[i] = akOutput[i].Y();
    }

    if ( bOwner )
        delete[] akOutput;

    // common triangulator for the interpolators
    m_pkDel = new Delaunay2<Real>(iQuantity,akInput,bOwner);

    // Create interpolator for x-coordinate of vector field.
    m_pkXInterp = new IntpQdrNonuniform2<Real>(*m_pkDel,afXOutput,true);

    // Create interpolator for y-coordinate of vector field, but share the
    // already created triangulation for the x-interpolator.
    m_pkYInterp = new IntpQdrNonuniform2<Real>(*m_pkDel,afYOutput,true);
}
//----------------------------------------------------------------------------
template <class Real>
IntpVectorField2<Real>::~IntpVectorField2 ()
{
    delete m_pkDel;
    delete m_pkXInterp;
    delete m_pkYInterp;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntpVectorField2<Real>::Evaluate (const Vector2<Real>& rkInput,
    Vector2<Real>& rkOutput)
{
    Real fXDeriv, fYDeriv;
    return m_pkXInterp->Evaluate(rkInput,rkOutput.X(),fXDeriv,fYDeriv)
        && m_pkYInterp->Evaluate(rkInput,rkOutput.Y(),fXDeriv,fYDeriv);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntpVectorField2<float>;

// double
template WM3_ITEM
class IntpVectorField2<double>;
//----------------------------------------------------------------------------
}
