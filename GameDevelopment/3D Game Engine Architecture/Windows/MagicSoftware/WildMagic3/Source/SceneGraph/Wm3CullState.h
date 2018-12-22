// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CULLSTATE_H
#define WM3CULLSTATE_H

#include "Wm3GlobalState.h"

namespace Wm3
{

class WM3_ITEM CullState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return CULL; }

    CullState ();
    virtual ~CullState ();

    enum // FrontType
    {
        FT_CCW,  // front faces are counterclockwise ordered
        FT_CW,   // front faces are clockwise ordered
        FT_QUANTITY
    };

    enum // CullType
    {
        CT_FRONT,  // cull front-facing triangles
        CT_BACK,   // cull back-facing triangles
        CT_QUANTITY
    };

    bool Enabled;   // default: true
    int FrontFace;  // default: FT_CCW
    int CullFace;   // default: CT_BACK
};

WM3_REGISTER_STREAM(CullState);
WM3_REGISTER_INITIALIZE(CullState);
WM3_REGISTER_TERMINATE(CullState);
typedef Pointer<CullState> CullStatePtr;

}

#endif
