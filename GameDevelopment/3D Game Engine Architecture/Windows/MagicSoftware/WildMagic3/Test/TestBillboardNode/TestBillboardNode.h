// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TESTBILLBOARDNODE_H
#define TESTBILLBOARDNODE_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class TestBillboardNode : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    TestBillboardNode ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();

    NodePtr m_spkScene;
    BillboardNodePtr m_spkBillboard0, m_spkBillboard1;
    WireframeStatePtr m_spkWireframe;

private:
    void TestStream ();
    void TestClone ();
};

WM3_REGISTER_INITIALIZE(TestBillboardNode);

#endif

