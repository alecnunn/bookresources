// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef DRAWIMPLICITSURFACE_H
#define DRAWIMPLICITSURFACE_H

#include "Wm3WindowApplication2.h"
#include "RayTracer.h"
using namespace Wm3;

class DrawImplicitSurface : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    DrawImplicitSurface ();

    virtual bool OnInitialize ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnSpecialKeyDown (int iKey, int iX, int iY);

protected:
    RayTracer m_kRT;
    int m_iMaxSample;
    bool m_bBlur;
};

WM3_REGISTER_INITIALIZE(DrawImplicitSurface);

#endif
