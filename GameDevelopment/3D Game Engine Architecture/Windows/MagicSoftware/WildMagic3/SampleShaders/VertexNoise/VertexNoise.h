// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef VERTEXNOISE_H
#define VERTEXNOISE_H

#include "Wm3WindowApplication3.h"
#include "Wm3VertexNoiseEffect.h"
using namespace Wm3;

class VertexNoise : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    VertexNoise ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void AttachShader (Spatial* pkObject);

    NodePtr m_spkScene;
    VertexNoiseEffectPtr m_spkEffect;

    void InitVertexNoiseConstants ();
    void UpdateConstants ();

    float m_fDisplacement,m_fNoiseScale;
    Vector4f m_kNoiseTrans;

    WireframeStatePtr m_spkWireframe;
};

WM3_REGISTER_INITIALIZE(VertexNoise);

#endif
