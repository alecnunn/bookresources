// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WATERDROPFORMATION_H
#define WATERDROPFORMATION_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class WaterDropFormation : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    WaterDropFormation ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    // scene graph
    void CreatePlane ();
    void CreateWall ();
    void CreateWaterRoot ();

    NodePtr m_spkScene, m_spkTrnNode, m_spkWaterRoot;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkPlane, m_spkWall;
    RevolutionSurfacePtr m_spkWaterSurface, m_spkWaterDrop;
    TexturePtr m_spkWater;

    // water sphere curves and simulation parameters
    void Configuration0 ();  // water surface
    void Configuration1 ();  // split into water surface and water drop
    void DoPhysical1 ();
    void DoPhysical2 ();
    void DoPhysical3 ();
    void DoPhysical ();
    void DoVisual ();

    NURBSCurve2f* m_pkSpline;
    NURBSCurve2f* m_pkCircle;
    Vector2f* m_akCtrlPoint;
    Vector2f* m_akTarget;
    float m_fSimTime, m_fSimDelta, m_fLastSeconds;
};

WM3_REGISTER_INITIALIZE(WaterDropFormation);

#endif

