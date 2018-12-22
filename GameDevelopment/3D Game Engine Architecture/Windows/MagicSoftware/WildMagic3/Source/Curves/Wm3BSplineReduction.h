// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BSPLINEREDUCTION_H
#define WM3BSPLINEREDUCTION_H

#include "Wm3Vector2.h"
#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real, class TVector>
class WM3_ITEM BSplineReduction
{
public:
    // The input quantity iQuantity must be 2 or larger.  The caller is
    // responsible for deleting the input array akCtrl and the output array
    // akCtrlOut.  The degree iDegree of the input curve must satisfy the
    // condition 1 <= iDegree <= iQuantity-1.  The degree of the output
    // curve is the same as that of the input curve.  The value fFraction
    // must be in [0,1].  If the fraction is 1, the output curve is identical
    // to the input curve.  If the fraction is too small to produce a valid
    // number of control points, the output control quantity is chosen to be
    // riQuantityOut = iDegree+1.

    BSplineReduction (int iQuantity, const TVector* akCtrl, int iDegree,
        Real fFraction, int& riQuantityOut, TVector*& rakCtrlOut);

    ~BSplineReduction ();

private:
    Real MinSupport (int iBasis, int i) const;
    Real MaxSupport (int iBasis, int i) const;
    Real F (int iBasis, int i, int j, Real fTime);
    static Real Integrand (Real fTime, void* pvThis);

    int m_iDegree;
    int m_aiQuantity[2];
    int m_aiNumKnots[2];  // N+D+2
    Real* m_aafKnot[2];

    // for the integration-based least-squares fitting
    int m_aiBasis[2], m_aiIndex[2];
};

typedef BSplineReduction<float,Vector2f> BSplineReduction2f;
typedef BSplineReduction<double,Vector2d> BSplineReduction2d;
typedef BSplineReduction<float,Vector3f> BSplineReduction3f;
typedef BSplineReduction<double,Vector3d> BSplineReduction3d;

}

#endif
