// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef BALLHILL_H
#define BALLHILL_H

#include "Wm3WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm3;

class BallHill : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    BallHill ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    TriMesh* CreateGround ();
    TriMesh* CreateHill ();
    TriMesh* CreateBall ();
    Polyline* CreatePath ();
    void DoPhysical ();
    Vector3f UpdateBall ();

    // scene graph
    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkBall;
    PolylinePtr m_spkPath;
    float m_fRadius;
    int m_iNextPoint;

    // physics
    PhysicsModule m_kModule;
};

WM3_REGISTER_INITIALIZE(BallHill);

#endif
