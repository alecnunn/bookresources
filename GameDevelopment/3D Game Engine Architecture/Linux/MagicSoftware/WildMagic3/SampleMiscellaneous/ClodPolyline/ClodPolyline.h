// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef CLODPOLYLINE_H
#define CLODPOLYLINE_H

#include "Wm3WindowApplication2.h"
#include "Polyline3.h"
using namespace Wm3;

class ClodPolyline : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    ClodPolyline ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    Polyline3* m_pkPolyline;
    int m_iLOD;
};

WM3_REGISTER_INITIALIZE(ClodPolyline);

#endif
