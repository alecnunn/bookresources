// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTPLINEARNONUNIFORM3_H
#define WM3INTPLINEARNONUNIFORM3_H

// Linear interpolation of a network of triangles whose vertices are of the
// form (x,y,z,f(x,y,z)).

#include "Wm3Delaunay3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntpLinearNonuniform3
{
public:
    // Construction and destruction.  If you want IntpLinearNonuniform3 to
    // delete the input array during destruction, set bOwner to 'true'.
    // Otherwise, you own the array and must delete it yourself.
    IntpLinearNonuniform3 (const Delaunay3<Real>& rkDT, Real* afF,
        bool bOwner);

    ~IntpLinearNonuniform3 ();

    // Linear interpolation.  The return value is 'true' if and only if the
    // input point is in the convex hull of the input vertices, in which case
    // the interpolation is valid.
    bool Evaluate (const Vector3<Real>& rkP, Real& rfF);

private:
    const Delaunay3<Real>& m_rkDT;
    Real* m_afF;
    bool m_bOwner;
};

typedef IntpLinearNonuniform3<float> IntpLinearNonuniform3f;
typedef IntpLinearNonuniform3<double> IntpLinearNonuniform3d;

}

#endif
