// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTTERRAIN_H
#define TESTTERRAIN_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestTerrain : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestTerrain ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateSkyDome ();
    void CreateTerrain ();

    // turret-based camera motion
    virtual void MoveForward ();
    virtual void MoveBackward ();
    virtual void MoveUp ();
    virtual void MoveDown ();

    NodePtr m_spkScene;
    TerrainPtr m_spkTerrain;
    TriMeshPtr m_spkSkyDome;
    WireframeStatePtr m_spkWireframe;

    float m_fUVBias;
    ColorRGBA m_kFogColor;
    float m_fHeightAboveTerrain;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestTerrain);

#endif
