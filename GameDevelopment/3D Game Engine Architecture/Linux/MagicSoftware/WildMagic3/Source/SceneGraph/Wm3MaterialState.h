// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MATERIALSTATE_H
#define WM3MATERIALSTATE_H

#include "Wm3GlobalState.h"
#include "Wm3ColorRGBA.h"

namespace Wm3
{

class WM3_ITEM MaterialState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return MATERIAL; }

    MaterialState ();
    virtual ~MaterialState ();

    ColorRGBA Emissive;  // default:  ColorRGBA(0,0,0,1)
    ColorRGBA Ambient;   // default:  ColorRGBA(0.2,0.2,0.2,1)
    ColorRGBA Diffuse;   // default:  ColorRGBA(0.8,0.8,0.8,1)
    ColorRGBA Specular;  // default:  ColorRGBA(0,0,0,1)
    float Shininess;     // default:  1
};

WM3_REGISTER_STREAM(MaterialState);
WM3_REGISTER_INITIALIZE(MaterialState);
WM3_REGISTER_TERMINATE(MaterialState);
typedef Pointer<MaterialState> MaterialStatePtr;

}

#endif
