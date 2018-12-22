// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef SIMPLEPATCH_H
#define SIMPLEPATCH_H

#include "Wm3SurfacePatch.h"

namespace Wm3
{

class WM3_ITEM SimplePatch : public SurfacePatch
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    SimplePatch ();
    virtual ~SimplePatch ();

    float Amplitude;

    // position and derivatives up to second order
    virtual Vector3f P (float fU, float fV) const;
    virtual Vector3f PU (float fU, float fV) const;
    virtual Vector3f PV (float fU, float fV) const;
    virtual Vector3f PUU (float fU, float fV) const;
    virtual Vector3f PUV (float fU, float fV) const;
    virtual Vector3f PVV (float fU, float fV) const;
};

WM3_REGISTER_STREAM(SimplePatch);
typedef Pointer<SimplePatch> SimplePatchPtr;

}

#endif
