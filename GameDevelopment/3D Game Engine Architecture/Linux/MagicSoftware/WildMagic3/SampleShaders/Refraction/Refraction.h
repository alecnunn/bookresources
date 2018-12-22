// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef REFRACTION_H
#define REFRACTION_H

#include "Wm3WindowApplication3.h"
#include "Wm3RefractionEffect.h"
using namespace Wm3;

class Refraction : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    Refraction ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();

    NodePtr m_spkScene, m_spkTrnNode;
    RefractionEffectPtr m_spkEffect;
    ScreenPolygonPtr m_spkSPoly;
    float m_fRIndex;
    bool m_bReflection;
};

WM3_REGISTER_INITIALIZE(Refraction);

#endif
