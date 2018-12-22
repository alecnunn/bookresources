// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef RIPPLINGOCEAN_H
#define RIPPLINGOCEAN_H

#include "Wm3WindowApplication3.h"
#include "Wm3RipplingOceanEffect.h"
using namespace Wm3;

class RipplingOcean : public WindowApplication3
{
    WM3_DECLARE_INITIALIZE;

public:
    RipplingOcean ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void AttachShader (Spatial* pkObject);

    NodePtr m_spkScene;
    NodePtr m_spkTrnNode;
    NodePtr m_spkModel;
    RipplingOceanEffectPtr m_spkEffect;

    TriMesh* CreateRectangleMesh (
        const Vector3f& rkCenter, const Vector3f& rkU, const Vector3f& rkV,
        const Vector3f& rkAxis, float fUExtent, float fVExtent,
        int iUSamples, int iVSamples, ColorRGBArray*& rpkOutputColors,
        Vector2fArray*& rpkOutputUVs);
    TriMesh* CreateRect (float fSize, float fDepth);
    void HeightToNormalMap (Image*& pkImage);

    void SetConstants ();

    // Time info
    bool m_bStopped;
    float m_fStopTime;

    // Wave info
    float m_fAmbient;
    float m_fTexRepeat;
    float m_fWaveSpeedFactor;
    float m_fWaveHeightFactor;
    float m_fRippleSpeedFactor;
};

WM3_REGISTER_INITIALIZE(RipplingOcean);

#endif
