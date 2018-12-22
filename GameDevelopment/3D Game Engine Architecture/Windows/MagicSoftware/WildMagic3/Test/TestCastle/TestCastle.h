// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTCASTLE_H
#define TESTCASTLE_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestCastle : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestCastle ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);

protected:
    // arrange for camera to be fixed distance above the nearest object
    void AdjustVerticalDistance ();
    virtual void MoveForward ();
    virtual void MoveBackward ();
    float m_fVerticalDistance;

    void SetAllWireframe (Spatial* pkSpatial);
    void ToggleAllWireframe (Spatial* pkSpatial);

    NodePtr m_spkScene;
    NodePtr m_spkModel;

    // name of picked object
    char m_acPicked[1024];
};

WM3_REGISTER_INITIALIZE(TestCastle);

#endif
