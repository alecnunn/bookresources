// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3FRUSTUM3_H
#define WM3FRUSTUM3_H

#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM Frustum3
{
public:
    // Orthogonal frustum.  Let E be the origin, D be the direction vector,
    // U be the up vector, and R be the right vector.  Let u > 0 and r > 0 be
    // the extents in the U and R directions, respectively.  Let n and f be
    // the extents in the D direction with 0 < n < f.  The four corners of the
    // frustum in the near plane are E + n*D + s0*u*U + s1*r*R where |s0| =
    // |s1| = 1 (four choices).  The four corners of the frustum in the far
    // plane are E + f*D + (f/n)*(s0*u*U + s1*r*R) where |s0| = |s1| = 1 (four
    // choices).

    // The default frustum is E = (0,0,0), D = (0,0,-1), U = (0,1,0), and
    // R = (1,0,0), DMin = 1, DMax = 2, UBound = 1, RBound = 1.
    Frustum3 ();
    Frustum3 (const Vector3<Real>& rkOrigin, const Vector3<Real>& rkDVector,
        const Vector3<Real>& rkUVector, const Vector3<Real>& rkRVector,
        Real fDMin, Real fDMax, Real fUBound, Real fRBound);

    // The Update() function must be called whenever changes are made to
    // DMin, DMac, UBound, or RBound.  The values DRatio, MTwoUF, and MTwoRF
    // are dependent on the changes, so call the Get accessors only after the
    // Update() call.
    void Update ();
    Real GetDRatio () const;
    Real GetMTwoUF () const;
    Real GetMTwoRF () const;

    void ComputeVertices (Vector3<Real> akVertex[8]) const;

    Vector3<Real> Origin, DVector, UVector, RVector;
    Real DMin, DMax, UBound, RBound;

protected:
    // derived quantities
    Real m_fDRatio, m_fMTwoUF, m_fMTwoRF;
};

#include "Wm3Frustum3.inl"

typedef Frustum3<float> Frustum3f;
typedef Frustum3<double> Frustum3d;

}

#endif
