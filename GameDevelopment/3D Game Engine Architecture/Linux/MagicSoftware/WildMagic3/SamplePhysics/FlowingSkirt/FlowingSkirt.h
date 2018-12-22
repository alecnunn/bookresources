// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef FLOWINGSKIRT_H
#define FLOWINGSKIRT_H

#include "Wm3WindowApplication3.h"
#include "Wm3BSplineCurve3.h"
using namespace Wm3;

class FlowingSkirt : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    FlowingSkirt ();
    virtual ~FlowingSkirt ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateSkirt ();
    void UpdateSkirt ();
    void ModifyCurves ();

    // the scene graph
    NodePtr m_spkScene, m_spkTrnNode;
    TriMeshPtr m_spkSkirt;
    WireframeStatePtr m_spkWireframe;

    // the skirt is a generalized Bezier cylinder
    int m_iNumCtrl, m_iDegree;
    float m_fATop, m_fBTop, m_fABot, m_fBBot;
    BSplineCurve3f* m_pkSkirtTop;
    BSplineCurve3f* m_pkSkirtBot;
    float* m_afFrequency;
};

WM3_REGISTER_INITIALIZE(FlowingSkirt);

#endif
