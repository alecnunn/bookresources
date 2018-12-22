// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTCOLLISION_H
#define TESTCOLLISION_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestCollision : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestCollision ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();

    NodePtr m_spkScene;
    TriMeshPtr m_spkCyln0, m_spkCyln1;
    WireframeStatePtr m_spkWireframe;

    // collision system
    CollisionGroup* m_pkGroup;
    bool Transform (unsigned char ucKey);
    void ResetColors ();
    static void Response (CollisionRecord& rkRecord0, int iT0,
        CollisionRecord& rkRecord1, int iT1,
        Intersector<float,Vector3f>* pkIntersector);

    static Vector2f ms_kBlueUV, ms_kRedUV, ms_kCyanUV, ms_kYellowUV;
};

WM3_REGISTER_INITIALIZE(TestCollision);

#endif

