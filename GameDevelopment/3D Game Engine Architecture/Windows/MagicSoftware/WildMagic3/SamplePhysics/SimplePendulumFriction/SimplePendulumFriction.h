// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef SIMPLEPENDULUMFRICTION_H
#define SIMPLEPENDULUMFRICTION_H

#include "Wm3WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm3;

class SimplePendulumFriction : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    SimplePendulumFriction ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    TriMesh* CreateFloor ();
    Polypoint* CreatePath ();
    Node* CreatePendulum ();
    void DoPhysical ();

    // the scene graph
    NodePtr m_spkScene, m_spkPendulum;
    WireframeStatePtr m_spkWireframe;

    // physics
    PhysicsModule m_kModule;

    // controlled frame rate
    float m_fLastIdle;
};

WM3_REGISTER_INITIALIZE(SimplePendulumFriction);

#endif
