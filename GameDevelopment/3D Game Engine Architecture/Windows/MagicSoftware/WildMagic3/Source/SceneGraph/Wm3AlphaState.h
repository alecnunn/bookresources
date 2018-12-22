// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3ALPHASTATE_H
#define WM3ALPHASTATE_H

#include "Wm3GlobalState.h"

namespace Wm3
{

class WM3_ITEM AlphaState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return ALPHA; }

    AlphaState ();
    virtual ~AlphaState ();

    enum // SrcBlend values
    {
        SBF_ZERO,
        SBF_ONE,
        SBF_DST_COLOR,
        SBF_ONE_MINUS_DST_COLOR,
        SBF_SRC_ALPHA,
        SBF_ONE_MINUS_SRC_ALPHA,
        SBF_DST_ALPHA,
        SBF_ONE_MINUS_DST_ALPHA,
        SBF_SRC_ALPHA_SATURATE,
        SBF_CONSTANT_COLOR,
        SBF_ONE_MINUS_CONSTANT_COLOR,
        SBF_CONSTANT_ALPHA,
        SBF_ONE_MINUS_CONSTANT_ALPHA,
        SBF_QUANTITY
    };

    enum // DstBlend values
    {
        DBF_ZERO,
        DBF_ONE,
        DBF_SRC_COLOR,
        DBF_ONE_MINUS_SRC_COLOR,
        DBF_SRC_ALPHA,
        DBF_ONE_MINUS_SRC_ALPHA,
        DBF_DST_ALPHA,
        DBF_ONE_MINUS_DST_ALPHA,
        DBF_CONSTANT_COLOR,
        DBF_ONE_MINUS_CONSTANT_COLOR,
        DBF_CONSTANT_ALPHA,
        DBF_ONE_MINUS_CONSTANT_ALPHA,
        DBF_QUANTITY
    };

    enum // Test values
    {
        TF_NEVER,
        TF_LESS,
        TF_EQUAL,
        TF_LEQUAL,
        TF_GREATER,
        TF_NOTEQUAL,
        TF_GEQUAL,
        TF_ALWAYS,
        TF_QUANTITY
    };

    bool BlendEnabled;  // default: false
    int SrcBlend;       // default: SBF_SRC_ALPHA
    int DstBlend;       // default: DBF_ONE_MINUS_SRC_ALPHA
    bool TestEnabled;   // default: false;
    int Test;           // default: TF_ALWAYS
    float Reference;    // default: 0, always in [0,1]
};

WM3_REGISTER_STREAM(AlphaState);
WM3_REGISTER_INITIALIZE(AlphaState);
WM3_REGISTER_TERMINATE(AlphaState);
typedef Pointer<AlphaState> AlphaStatePtr;

}

#endif
