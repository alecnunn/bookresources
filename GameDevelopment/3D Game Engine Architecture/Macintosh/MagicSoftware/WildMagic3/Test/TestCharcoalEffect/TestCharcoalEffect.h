// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTCHARCOALEFFECT_H
#define TESTCHARCOALEFFECT_H

#include "Wm3WindowApplication3.h"
#include "Wm3CharcoalEffect.h"
using namespace Wm3;

class TestCharcoalEffect : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestCharcoalEffect ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void AttachShader (Spatial* pkObject);
    void UpdateConstants ();

    NodePtr m_spkScene;
    CharcoalEffectPtr m_spkEffect;
    bool m_bSmudge, m_bDisplayPaper, m_bDisplayLighting;

private:
    void TestStream ();
};

WM3_REGISTER_INITIALIZE(TestCharcoalEffect);

#endif
