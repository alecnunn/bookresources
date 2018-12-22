// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef BSPLINELEASTSQUARES_H
#define BSPLINELEASTSQUARES_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class BSplineLeastSquares : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    BSplineLeastSquares ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    Polyline* OriginalPolyline (int iCtrlQuantity, Vector3d* akCtrl);
    Polyline* ReducedPolyline (int iCtrlQuantity, Vector3d* akCtrl,
        double dFraction);

    NodePtr m_spkScene, m_spkTrnNode;
    int m_iDegree;
};

WM3_REGISTER_INITIALIZE(BSplineLeastSquares);

#endif
