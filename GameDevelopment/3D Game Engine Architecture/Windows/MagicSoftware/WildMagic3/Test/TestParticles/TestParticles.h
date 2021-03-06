// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTPARTICLES_H
#define TESTPARTICLES_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestParticles : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestParticles ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    Image* CreateImage ();
    Particles* CreateParticles ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestParticles);

#endif
