// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTCLODTERRAIN_H
#define TESTCLODTERRAIN_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestClodTerrain : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestClodTerrain ();

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
    virtual void TurnLeft ();
    virtual void TurnRight ();
    virtual void LookUp ();
    virtual void LookDown ();

    NodePtr m_spkScene;
    ClodTerrainPtr m_spkTerrain;
    TriMeshPtr m_spkSkyDome;
    WireframeStatePtr m_spkWireframe;

    float m_fUVBias;
    ColorRGBA m_kFogColor;
    float m_fHeightAboveTerrain;

    // override the simplification by setting to 'false'
    bool m_bAllowSimplification;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestClodTerrain);

#endif
