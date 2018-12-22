// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3FRESNELEFFECT_H
#define WM3FRESNELEFFECT_H

#include "Wm3ShaderEffect.h"

namespace Wm3
{

class WM3_ITEM FresnelEffect : public ShaderEffect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    FresnelEffect ();
    virtual ~FresnelEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();


protected:
    // streaming
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(FresnelEffect);
typedef Pointer<FresnelEffect> FresnelEffectPtr;

}

#endif
