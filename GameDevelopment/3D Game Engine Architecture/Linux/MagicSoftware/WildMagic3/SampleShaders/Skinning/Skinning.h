// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef SKINNING_H
#define SKINNING_H

#include "Wm3WindowApplication3.h"
#include "Wm3SkinningEffect.h"
using namespace Wm3;

class Skinning : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    Skinning ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();
    void AttachShader (Spatial* pkObject);

    NodePtr m_spkScene;
    TriMeshPtr m_spkMesh;
    SkinningEffectPtr m_spkEffect;

    void UpdateConstants (float fTime);
    void InitializeEffect (int iRadialSamples, float fRadius, float fHeight);
};

WM3_REGISTER_INITIALIZE(Skinning);

#endif
