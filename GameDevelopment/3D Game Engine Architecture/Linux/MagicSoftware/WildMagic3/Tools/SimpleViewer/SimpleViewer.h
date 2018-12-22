// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef SIMPLEVIEWER_H
#define SIMPLEVIEWER_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class SimpleViewer : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    SimpleViewer ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);

protected:
    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;

    // animation
    double m_dAnimTime, m_dAnimDelta;
    bool m_bDoAnimation;

    // name of picked object
    char m_acPicked[1024];
};

WM3_REGISTER_INITIALIZE(SimpleViewer);

#endif

