// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SHADESTATE_H
#define WM3SHADESTATE_H

#include "Wm3GlobalState.h"

namespace Wm3
{

class WM3_ITEM ShadeState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return SHADE; }

    ShadeState ();
    virtual ~ShadeState ();

    enum // ShadeMode
    {
        SM_FLAT,
        SM_SMOOTH,
        SM_QUANTITY
    };

    int Shade;  // default: SM_SMOOTH
};

WM3_REGISTER_STREAM(ShadeState);
WM3_REGISTER_INITIALIZE(ShadeState);
WM3_REGISTER_TERMINATE(ShadeState);
typedef Pointer<ShadeState> ShadeStatePtr;

}

#endif
