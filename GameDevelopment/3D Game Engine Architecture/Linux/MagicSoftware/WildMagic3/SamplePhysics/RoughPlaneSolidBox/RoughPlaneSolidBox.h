// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef ROUGHPLANESOLIDBOX_H
#define ROUGHPLANESOLIDBOX_H

#include "Wm3WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm3;

class RoughPlaneSolidBox : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    RoughPlaneSolidBox ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    Node* CreateBox ();
    TriMesh* CreateRamp ();
    TriMesh* CreateGround();
    void InitializeModule ();
    void MoveBox ();
    void DoPhysical ();

    NodePtr m_spkScene, m_spkBox;
    WireframeStatePtr m_spkWireframe;

    PhysicsModule m_kModule;
    bool m_bDoUpdate;
};

WM3_REGISTER_INITIALIZE(RoughPlaneSolidBox);

#endif
