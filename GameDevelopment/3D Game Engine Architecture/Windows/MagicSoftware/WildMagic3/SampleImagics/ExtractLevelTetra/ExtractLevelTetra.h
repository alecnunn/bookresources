// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef EXTRACTLEVELTETRA_H
#define EXTRACTLEVELTETRA_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class ExtractLevelTetra : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    ExtractLevelTetra ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    bool CreateModel ();

    NodePtr m_spkScene, m_spkTrnNode;
    TriMeshPtr m_spkModel;
    WireframeStatePtr m_spkWireframe;
};

WM3_REGISTER_INITIALIZE(ExtractLevelTetra);

#endif
