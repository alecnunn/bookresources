// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3ZBUFFERSTATE_H
#define WM3ZBUFFERSTATE_H

#include "Wm3GlobalState.h"

namespace Wm3
{

class WM3_ITEM ZBufferState : public GlobalState
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;
    WM3_DECLARE_INITIALIZE;
    WM3_DECLARE_TERMINATE;

public:
    virtual int GetGlobalStateType () const { return ZBUFFER; }

    ZBufferState ();
    virtual ~ZBufferState ();

    enum // Compare values
    {
        CF_NEVER,
        CF_LESS,
        CF_EQUAL,
        CF_LEQUAL,
        CF_GREATER,
        CF_NOTEQUAL,
        CF_GEQUAL,
        CF_ALWAYS,
        CF_QUANTITY
    };

    bool Enabled;    // default: true
    bool Writable;   // default: true
    int Compare;     // default: CF_LEQUAL
};

WM3_REGISTER_STREAM(ZBufferState);
WM3_REGISTER_INITIALIZE(ZBufferState);
WM3_REGISTER_TERMINATE(ZBufferState);
typedef Pointer<ZBufferState> ZBufferStatePtr;

}

#endif
