// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef VOLUMERENDER_H
#define VOLUMERENDER_H

#include "Wm3WindowApplication2.h"
#include "RayTrace.h"
using namespace Wm3;

class VolumeRender : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    VolumeRender ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnMotion (int iButton, int iX, int iY);

protected:
    int m_iBound;
    RayTrace* m_pkRT;
    float m_fX0, m_fY0, m_fX1, m_fY1, m_fHBound, m_fGamma;
    bool m_bButtonDown;
};

WM3_REGISTER_INITIALIZE(VolumeRender);

#endif
