// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef FREETOPFIXEDTIP_H
#define FREETOPFIXEDTIP_H

#include "Wm3WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm3;

class FreeTopFixedTip : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    FreeTopFixedTip ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    // scene graph
    void CreateScene ();
    TriMesh* CreateFloor ();
    TriMesh* CreateTop ();
    Polyline* CreateAxisTop ();
    Polyline* CreateAxisVertical ();

    NodePtr m_spkScene, m_spkTopRoot;
    WireframeStatePtr m_spkWireframe;
    float m_fMaxPhi;

    // physics
    void DoPhysical ();
    PhysicsModule m_kModule;

    // controlled frame rate
    float m_fLastIdle;
};

WM3_REGISTER_INITIALIZE(FreeTopFixedTip);

#endif
