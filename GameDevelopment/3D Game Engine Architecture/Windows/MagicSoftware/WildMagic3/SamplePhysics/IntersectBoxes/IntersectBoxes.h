// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef INTERSECTBOXES_H
#define INTERSECTBOXES_H

#include "Wm3WindowApplication3.h"
using namespace Wm3;

class IntersectBoxes : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    IntersectBoxes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void ModifyBoxes ();
    void ModifyMesh (int i);

    std::vector<AxisAlignedBox3f> m_kBoxes;
    IntersectingBoxesf* m_pkIB;
    bool m_bDoSimulation;
    float m_fLastIdle;

    NodePtr m_spkScene;
    MaterialStatePtr m_spkBlue, m_spkRed;
    WireframeStatePtr m_spkWireframe;

    static float ms_fSize;
};

WM3_REGISTER_INITIALIZE(IntersectBoxes);

#endif
