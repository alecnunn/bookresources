// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef BSPLINECURVEEXAMPLES_H
#define BSPLINECURVEEXAMPLES_H

#include "Wm3WindowApplication2.h"
#include "Wm3BSplineCurve2.h"
using namespace Wm3;

class BSplineCurveExamples : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    BSplineCurveExamples ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    BSplineCurve2f* m_pkSpline;
    int m_iNumCtrlPoints, m_iDegree;
    Vector2f* m_akCtrlPoint;
    float* m_afKnot;
    float m_afLocCtrlMin[6], m_afLocCtrlMax[6];
    int m_iCurveType;
    bool m_bModified;
};

WM3_REGISTER_INITIALIZE(BSplineCurveExamples);

#endif


