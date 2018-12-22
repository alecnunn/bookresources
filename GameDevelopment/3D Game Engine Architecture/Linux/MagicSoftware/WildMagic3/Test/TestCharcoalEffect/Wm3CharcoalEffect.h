// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CHARCOALEFFECT_H
#define WM3CHARCOALEFFECT_H

#include "Wm3ShaderEffect.h"
#include "Wm3CharcoalVShader.h"
#include "Wm3CharcoalPShader.h"

namespace Wm3
{

class WM3_ITEM CharcoalEffect : public ShaderEffect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // The vertex shader expects two lights.  They should be attached
    // somewhere in the scene to affect the Geometry object to which this
    // effect is attached.
    //
    // You must generate the contrast texture coordinates for each Geometry
    // object to which this is attached.  The coordinates are random.
    CharcoalEffect (Image* pkContrastTexture, Image* pkRandomTexture,
        Image* pkPaperTexture);
    virtual ~CharcoalEffect ();

    void SetContrastTextureCoordinates (int iVQuantity);

    // create a clone of the effect
    virtual Effect* Clone ();

    void SetAmbientIntensity (float fAmbientIntensity);
    float GetAmbientIntensity () const;
    void SetContrastExponent (float fContrastExponent);
    float GetContrastExponent () const;
    void SetSmudgeFactor (float fSmudgeFactor);
    float GetSmudgeFactor () const;
    void SetPaperFactor (float fPaperFactor);
    float GetPaperFactor () const;

protected:
    // streaming
    CharcoalEffect ();
    virtual void DoCloning (Effect* pkClone);

    void GenerateContrastImage ();
};

WM3_REGISTER_STREAM(CharcoalEffect);
typedef Pointer<CharcoalEffect> CharcoalEffectPtr;

#include "Wm3CharcoalEffect.inl"

}

#endif
