// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3VERTEXCOLOREFFECT_H
#define WM3VERTEXCOLOREFFECT_H

#include "Wm3Effect.h"
#include "Wm3ColorRGBArray.h"
#include "Wm3ColorRGBAArray.h"

namespace Wm3
{

class WM3_ITEM VertexColorEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    VertexColorEffect (ColorRGBArray* pkColorRGBs);
    VertexColorEffect (ColorRGBAArray* pkColorRGBAs);
    virtual ~VertexColorEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

protected:
    VertexColorEffect ();
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(VertexColorEffect);
typedef Pointer<VertexColorEffect> VertexColorEffectPtr;

}

#endif
