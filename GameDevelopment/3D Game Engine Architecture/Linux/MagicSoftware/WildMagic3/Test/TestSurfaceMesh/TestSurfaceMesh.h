// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTSURFACEMESH_H
#define TESTSURFACEMESH_H

#include "Wm3WindowApplication3.h"
#include "SimpleSegment.h"
#include "SimplePatch.h"
using namespace Wm3;

class TestSurfaceMesh : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestSurfaceMesh ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void CreateSimpleSegment ();
    void CreateSimplePatch ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;

    SimpleSegmentPtr m_spkSegment;
    CurveMeshPtr m_spkCurve;

    SimplePatchPtr m_spkPatch;
    SurfaceMeshPtr m_spkSurface;

    int m_iLevel;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestSurfaceMesh);

#endif

