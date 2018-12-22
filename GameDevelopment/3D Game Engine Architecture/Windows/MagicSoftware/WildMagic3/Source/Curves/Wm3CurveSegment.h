// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CURVESEGMENT_H
#define WM3CURVESEGMENT_H

#include "Wm3Object.h"
#include "Wm3Vector3.h"

namespace Wm3
{

class WM3_ITEM CurveSegment : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~CurveSegment ();

    // The parametric domain is umin <= u <= umax.
    float GetUMin () const;
    float GetUMax () const;

    // position and derivatives up to third order
    virtual Vector3f P (float fU) const = 0;
    virtual Vector3f PU (float fU) const = 0;
    virtual Vector3f PUU (float fU) const = 0;
    virtual Vector3f PUUU (float fU) const = 0;

    // differential geometric quantities
    Vector3f Tangent (float fU) const;
    Vector3f Normal (float fU) const;
    Vector3f Binormal (float fU) const;
    void GetFrame (float fU, Vector3f& rkPosition, Vector3f& rkTangent,
        Vector3f& rkNormal, Vector3f& rkBinormal) const;
    float Curvature (float fU) const;
    float Torsion (float fU) const;

protected:
    CurveSegment (float fUMin, float fUMax);
    CurveSegment ();

    float m_fUMin, m_fUMax;
};

WM3_REGISTER_STREAM(CurveSegment);
typedef Pointer<CurveSegment> CurveSegmentPtr;
#include "Wm3CurveSegment.inl"

}

#endif
