// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTINVERSEKINEMATICS_H
#define TESTINVERSEKINEMATICS_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestInverseKinematics : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestInverseKinematics ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMotion (int iButton, int iX, int iY);

protected:
    TriMesh* CreateCube ();
    Polyline* CreateRod ();
    TriMesh* CreateGround ();
    void UpdateRod ();
    bool Transform (unsigned char ucKey);

    NodePtr m_spkScene, m_spkIKSystem, m_spkGoal, m_spkJoint0, m_spkJoint1;
    WireframeStatePtr m_spkWireframe;
    PolylinePtr m_spkRod;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestInverseKinematics);

#endif
