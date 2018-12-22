// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3FOGSTATE_H
#define WM3FOGSTATE_H

#include "Wm3GlobalState.h"
#include "Wm3ColorRGBA.h"

namespace Wm3
{

class WM3_ITEM FogState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return FOG; }

    FogState ();
    virtual ~FogState ();

    enum // DensityFunction
    {
        DF_LINEAR,
        DF_EXP,
        DF_EXPSQR,
        DF_QUANTITY
    };

    enum // ApplyFunction
    {
        AF_PER_VERTEX,
        AF_PER_PIXEL,
        AF_QUANTITY
    };

    bool Enabled;         // default: false
    float Start;          // default: 0
    float End;            // default: 1
    float Density;        // default: 1
    ColorRGBA Color;      // default: ColorRGB(0,0,0)
    int DensityFunction;  // default: DF_LINEAR
    int ApplyFunction;    // default: AF_PER_VERTEX
};

WM3_REGISTER_STREAM(FogState);
WM3_REGISTER_INITIALIZE(FogState);
WM3_REGISTER_TERMINATE(FogState);
typedef Pointer<FogState> FogStatePtr;

}

#endif
