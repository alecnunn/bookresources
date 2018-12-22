// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PROJECTEDTEXTUREEFFECT_H
#define WM3PROJECTEDTEXTUREEFFECT_H

#include "Wm3Effect.h"
#include "Wm3Camera.h"

namespace Wm3
{

class WM3_ITEM ProjectedTextureEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    ProjectedTextureEffect (Image* pkImage, Camera* pkCamera,
        int iApplyMode);
    virtual ~ProjectedTextureEffect ();

    // Create a clone of the effect.  The camera is shared.
    virtual Effect* Clone ();

    // member access
    Camera* GetCamera () const;

protected:
    ProjectedTextureEffect ();
    virtual void DoCloning (Effect* pkClone);

    CameraPtr m_spkCamera;
};

WM3_REGISTER_STREAM(ProjectedTextureEffect);
typedef Pointer<ProjectedTextureEffect> ProjectedTextureEffectPtr;
#include "Wm3ProjectedTextureEffect.inl"

}

#endif
