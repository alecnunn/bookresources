// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef INTERSECTRECTANGLES_H
#define INTERSECTRECTANGLES_H

#include "Wm3WindowApplication2.h"
using namespace Wm3;

class IntersectRectangles : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    IntersectRectangles ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void ModifyRectangles ();
    void DrawRectangles ();

    std::vector<AxisAlignedBox2f> m_kRects;
    IntersectingRectanglesf* m_pkIR;
    bool m_bMouseDown;
    float m_fLastIdle;
};

WM3_REGISTER_INITIALIZE(IntersectRectangles);

#endif
