// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SURFACEPATCH_H
#define WM3SURFACEPATCH_H

#include "Wm3Object.h"
#include "Wm3Vector3.h"

namespace Wm3
{

class WM3_ITEM SurfacePatch : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~SurfacePatch ();

    // The parametric domain is either rectangular or triangular.  Valid (u,v)
    // values for a rectangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax
    // Valid (u,v) values for a triangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax,
    //   (vmax-vmin)*(u-umin)+(umax-umin)*(v-vmax) <= 0
    float GetUMin () const;
    float GetUMax () const;
    float GetVMin () const;
    float GetVMax () const;
    bool IsRectangular () const;

    // position and derivatives up to second order
    virtual Vector3f P (float fU, float fV) const = 0;
    virtual Vector3f PU (float fU, float fV) const = 0;
    virtual Vector3f PV (float fU, float fV) const = 0;
    virtual Vector3f PUU (float fU, float fV) const = 0;
    virtual Vector3f PUV (float fU, float fV) const = 0;
    virtual Vector3f PVV (float fU, float fV) const = 0;

    // Compute a coordinate frame.  The set {T0,T1,N} is a right-handed
    // orthonormal set.
    void GetFrame (float fU, float fV, Vector3f& rkPosition,
        Vector3f& rkTangent0, Vector3f& rkTangent1, Vector3f& rkNormal) const;

    // Differential geometric quantities.  The returned scalars are the
    // principal curvatures and the returned vectors are the corresponding
    // principal directions.
    void ComputePrincipalCurvatureInfo (float fU, float fV, float& rfCurv0,
        float& rfCurv1, Vector3f& rkDir0, Vector3f& rkDir1);

protected:
    SurfacePatch (float fUMin, float fUMax, float fVMin, float fVMax,
        bool bRectangular);
    SurfacePatch ();

    float m_fUMin, m_fUMax, m_fVMin, m_fVMax;
    bool m_bRectangular;
};

WM3_REGISTER_STREAM(SurfacePatch);
typedef Pointer<SurfacePatch> SurfacePatchPtr;
#include "Wm3SurfacePatch.inl"

}

#endif
