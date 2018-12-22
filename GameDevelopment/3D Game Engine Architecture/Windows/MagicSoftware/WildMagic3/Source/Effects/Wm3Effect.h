// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3EFFECT_H
#define WM3EFFECT_H

#include "Wm3Object.h"
#include "Wm3Renderer.h"
#include "Wm3ColorRGBArray.h"
#include "Wm3ColorRGBAArray.h"
#include "Wm3Texture.h"
#include "Wm3Vector2Array.h"

namespace Wm3
{

class WM3_ITEM Effect : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // The base class is not abstract.  This allows you to create an Effect
    // object and set the colors and textures as desired.  In particular, if
    // you have a special effect that involves fancy use of the "combine"
    // capabilities of textures, you can do this without having to derive
    // a class from Effect to manage the new feature.
    Effect ();
    virtual ~Effect ();

    // Create a clone of the effect.  Colors and textures are shared.  Each
    // derived class can override this behavior and decide what is copied
    // and what is shared.
    virtual Effect* Clone ();

    // data common to many effects
    ColorRGBArrayPtr ColorRGBs;
    ColorRGBAArrayPtr ColorRGBAs;
    TArray<TexturePtr> Textures;
    TArray<Vector2fArrayPtr> UVs;

protected:
    virtual void DoCloning (Effect* pkClone);

// internal use
public:
    // function required to draw the effect
    Renderer::DrawFunction Draw;
};

WM3_REGISTER_STREAM(Effect);
typedef Pointer<Effect> EffectPtr;

}

#endif
