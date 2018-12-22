// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTPBICUBIC2_H
#define WM3INTPBICUBIC2_H

#include "Wm3System.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntpBicubic2
{
public:
    // Construction and destruction.  IntpBicubic2 does not accept
    // responsibility for deleting the input array.  The application must do
    // so.  The interpolator is for uniformly spaced (x,y)-values.  The
    // function values are assumed to be organized as f(x,y) = F[y][x].
    // Exact interpolation is achieved by setting bCatmullRom to 'true',
    // giving you the Catmull-Rom blending matrix.  If a smooth interpolation
    // is desired, set bCatmullRom to 'false' to obtain B-spline blending.

    IntpBicubic2 (int iXBound, int iYBound, Real fXMin, Real fXSpacing,
        Real fYMin, Real fYSpacing, Real** aafF, bool bCatmullRom);

    int GetXBound () const;
    int GetYBound () const;
    int GetQuantity () const;
    Real** GetF () const;

    Real GetXMin () const;
    Real GetXMax () const;
    Real GetXSpacing () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetYSpacing () const;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax and ymin <= y <= ymax.
    // If (x,y) is outside the extremes, the function returns MAXREAL.  The
    // first operator is for function evaluation.  The second operator is for
    // function or derivative evaluations.  The uiXOrder argument is the order
    // of the x-derivative and the uiYOrder argument is the order of the
    // y-derivative.  Both orders are zero to get the function value itself.
    Real operator() (Real fX, Real fY) const;
    Real operator() (int iXOrder, int iYOrder, Real fX, Real fY) const;

private:
    int m_iXBound, m_iYBound, m_iQuantity;
    Real m_fXMin, m_fXMax, m_fXSpacing, m_fInvXSpacing;
    Real m_fYMin, m_fYMax, m_fYSpacing, m_fInvYSpacing;
    Real** m_aafF;
    const Real (*m_aafBlend)[4];

    static const Real ms_aafCRBlend[4][4];
    static const Real ms_aafBSBlend[4][4];
};

typedef IntpBicubic2<float> IntpBicubic2f;
typedef IntpBicubic2<double> IntpBicubic2d;

}

#endif
