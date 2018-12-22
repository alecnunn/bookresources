// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3WIREFRAMESTATE_H
#define WM3WIREFRAMESTATE_H

#include "Wm3GlobalState.h"

namespace Wm3
{

class WM3_ITEM WireframeState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return WIREFRAME; }

    WireframeState ();
    virtual ~WireframeState ();

    bool Enabled;  // default: false
};

WM3_REGISTER_STREAM(WireframeState);
WM3_REGISTER_INITIALIZE(WireframeState);
WM3_REGISTER_TERMINATE(WireframeState);
typedef Pointer<WireframeState> WireframeStatePtr;

}

#endif
