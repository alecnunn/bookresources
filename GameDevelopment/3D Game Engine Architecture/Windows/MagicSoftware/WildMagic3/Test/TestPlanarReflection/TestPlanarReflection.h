// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTPLANARREFLECTION_H
#define TESTPLANARREFLECTION_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestPlanarReflection : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestPlanarReflection ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void LoadBiped ();
    void CreatePlanes ();
    void CreatePlanarReflection ();

    NodePtr m_spkScene;
    NodePtr m_spkBiped;
    TriMeshPtr m_spkPlane0, m_spkPlane1;
    WireframeStatePtr m_spkWireframe;

    double m_dAnimTime, m_dAnimTimeDelta;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestPlanarReflection);

#endif
