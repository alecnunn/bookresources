// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DITHERSTATE_H
#define WM3DITHERSTATE_H

#include "Wm3GlobalState.h"

namespace Wm3
{

class WM3_ITEM DitherState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return DITHER; }

    DitherState ();
    virtual ~DitherState ();

    bool Enabled;  // default: false
};

WM3_REGISTER_STREAM(DitherState);
WM3_REGISTER_INITIALIZE(DitherState);
WM3_REGISTER_TERMINATE(DitherState);
typedef Pointer<DitherState> DitherStatePtr;

}

#endif
