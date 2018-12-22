// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef HELIXTUBESURFACE_H
#define HELIXTUBESURFACE_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class HelixTubeSurface : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    HelixTubeSurface ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnSpecialKeyDown (int iKey, int iX, int iY);

protected:
    MultipleCurve3f* CreateCurve ();
    static float Radial (float fT);
    virtual bool MoveCamera ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    MultipleCurve3f* m_pkCurve;
    float m_fMinCurveTime, m_fMaxCurveTime, m_fCurvePeriod;
    float m_fCurveTime, m_fDeltaTime;
};

WM3_REGISTER_INITIALIZE(HelixTubeSurface);

#endif
