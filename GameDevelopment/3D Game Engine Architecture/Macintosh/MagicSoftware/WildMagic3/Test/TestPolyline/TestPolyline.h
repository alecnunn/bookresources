// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTPOLYLINE_H
#define TESTPOLYLINE_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestPolyline : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestPolyline ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    NodePtr m_spkScene;
    PolylinePtr m_spkPolyline;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestPolyline);

#endif
