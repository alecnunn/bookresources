// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef ROUGHPLANEPARTICLE2_H
#define ROUGHPLANEPARTICLE2_H

#include "Wm3WindowApplication2.h"
#include "PhysicsModule.h"
using namespace Wm3;

class RoughPlaneParticle2 : public WindowApplication2
{
    WM3_DECLARE_INITIALIZE;

public:
    RoughPlaneParticle2 ();

    virtual bool OnInitialize ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    PhysicsModule m_kModule;
    int m_iIteration, m_iMaxIteration;
};

WM3_REGISTER_INITIALIZE(RoughPlaneParticle2);

#endif
