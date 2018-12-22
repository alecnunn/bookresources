// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTPSPHERE2_H
#define WM3INTPSPHERE2_H

// Interpolation of a scalar-valued function defined on a sphere.  Although
// the sphere lives in 3D, the interpolation is a 2D method whose input
// points are angles (theta,phi) from spherical coordinates.  The domains of
// the angles are -PI <= theta <= PI and 0 <= phi <= PI.

#include "Wm3IntpQdrNonuniform2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntpSphere2
{
public:
    // Construction and destruction.  If you want IntpSphere2 to delete the
    // input arrays during destruction, set bOwner to 'true'.  Otherwise, you
    // own the arrays and must delete them yourself.
    //
    // For complete spherical coverage, include the two antipodal (theta,phi)
    // points (-PI,0,F(-PI,0)) and (-PI,PI,F(-PI,PI)) in the input data.
    // These correspond to the sphere poles x = 0, y = 0, and |z| = 1.
    IntpSphere2 (int iQuantity, Real* afTheta, Real* afPhi, Real* afF,
        bool bOwner);

    ~IntpSphere2 ();

    // Spherical coordinates are
    //   x = cos(theta)*sin(phi)
    //   y = sin(theta)*sin(phi)
    //   z = cos(phi)
    // for -PI <= theta <= PI, 0 <= phi <= PI.  The application can use this
    // function to convert unit length vectors (x,y,z) to (theta,phi).
    static void GetSphericalCoords (Real fX, Real fY, Real fZ,
        Real& rfTheta, Real& rfPhi);

    bool Evaluate (Real fTheta, Real fPhi, Real& rfF);

private:
    Delaunay2<Real>* m_pkDel;
    IntpQdrNonuniform2<Real>* m_pkInterp;
};

typedef IntpSphere2<float> IntpSphere2f;
typedef IntpSphere2<double> IntpSphere2d;

}

#endif
