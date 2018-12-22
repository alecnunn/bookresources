// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTMORPHCONTROLLER_H
#define TESTMORPHCONTROLLER_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestMorphController : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestMorphController ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);

protected:
    void LoadTargets ();
    void CreateFace ();
    void CreateController ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkFace;

    // shared data
    IntArrayPtr m_spkIndices;
    MaterialStatePtr m_spkMaterial;
    LightPtr m_spkDLight;

    enum
    {
        TARGET_QUANTITY = 5,
        KEY_QUANTITY = 6
    };
    TriMeshPtr m_aspkTarget[TARGET_QUANTITY];

    // animation time
    double m_dBaseTime, m_dCurrTime;

    // picking
    int m_iXPick, m_iYPick;
    char m_acPickMessage[128];
    bool m_bPickPending;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestMorphController);

#endif
