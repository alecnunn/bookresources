// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef NURBSCURVEEXAMPLE_H
#define NURBSCURVEEXAMPLE_H

#include "Wm3WindowApplication2.h"
using namespace Wm3;

class NURBSCurveExample : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    NURBSCurveExample ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void DoSimulation1 ();
    void DoSimulation2 ();
    void InitialConfiguration ();
    void NextConfiguration ();

    NURBSCurve2f* m_pkSpline;
    NURBSCurve2f* m_pkCircle;
    Vector2f* m_akCtrlPoint;
    Vector2f* m_akTarget;
    float m_fH, m_fD;
    float m_fSimTime, m_fSimDelta;

    bool m_bDrawControlPoints;
};

WM3_REGISTER_INITIALIZE(NURBSCurveExample);

#endif


