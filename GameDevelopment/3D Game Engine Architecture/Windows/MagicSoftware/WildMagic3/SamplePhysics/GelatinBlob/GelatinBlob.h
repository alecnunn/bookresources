// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef GELATINBLOB_H
#define GELATINBLOB_H

#include "Wm3WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm3;

class GelatinBlob : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    GelatinBlob ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    // a mass-spring system
    PhysicsModule* m_pkModule;
    void DoPhysical ();

    // scene graph
    void CreateSphere ();
    void CreateSprings ();
    void CreateSegments ();
    void CreateScene ();
    NodePtr m_spkScene, m_spkTrnNode, m_spkSegments;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkSphere;
};

WM3_REGISTER_INITIALIZE(GelatinBlob);

#endif
