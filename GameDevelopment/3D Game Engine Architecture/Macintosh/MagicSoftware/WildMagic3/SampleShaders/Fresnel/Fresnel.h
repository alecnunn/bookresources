// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef FRESNEL_H
#define FRESNEL_H

#include "Wm3WindowApplication3.h"
#include "Wm3FresnelEffect.h"
using namespace Wm3;

class Fresnel : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    Fresnel ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void AttachShader (Spatial* pkObject);

    NodePtr m_spkScene;
    FresnelEffectPtr m_spkEffect;
    AlphaStatePtr m_spkAlpha;
};

WM3_REGISTER_INITIALIZE(Fresnel);

#endif
