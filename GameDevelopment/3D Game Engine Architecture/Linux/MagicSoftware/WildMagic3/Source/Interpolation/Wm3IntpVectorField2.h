// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTPVECTORFIELD2_H
#define WM3INTPVECTORFIELD2_H

// Given points (x0[i],y0[i]) which are mapped to (x1[i],y1[i]) for
// 0 <= i < N, interpolate positions (xIn,yIn) to (xOut,yOut).

#include "Wm3IntpQdrNonuniform2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntpVectorField2
{
public:
    // Construction and destruction.  If you want IntpVectorField2 to delete
    // the input arrays during destruction, set bOwner to 'true'.  Otherwise,
    // you own the arrays and must delete them yourself.
    IntpVectorField2 (int iQuantity, Vector2<Real>* akDomain,
        Vector2<Real>* akRange, bool bOwner);

    ~IntpVectorField2 ();

    // Return 'true' if and only if (xIn,yIn) is in the convex hull of the
    // input points.  In this case, (xOut,yOut) is valid.  If the return
    // value is 'false', (xOut,yOut) is invalid and should not be used.
    bool Evaluate (const Vector2<Real>& rkInput, Vector2<Real>& rkOutput); 

protected:
    Delaunay2<Real>* m_pkDel;
    IntpQdrNonuniform2<Real>* m_pkXInterp;
    IntpQdrNonuniform2<Real>* m_pkYInterp;
};

typedef IntpVectorField2<float> IntpVectorField2f;
typedef IntpVectorField2<double> IntpVectorField2d;

}

#endif
