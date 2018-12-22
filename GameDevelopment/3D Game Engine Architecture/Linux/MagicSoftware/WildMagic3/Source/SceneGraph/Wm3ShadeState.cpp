// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ShadeState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ShadeState,GlobalState);
WM3_IMPLEMENT_STREAM(ShadeState);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ShadeState,GlobalState);
WM3_IMPLEMENT_INITIALIZE(ShadeState);
WM3_IMPLEMENT_TERMINATE(ShadeState);

//----------------------------------------------------------------------------
void ShadeState::Initialize ()
{
    Default[SHADE] = new ShadeState;
}
//----------------------------------------------------------------------------
void ShadeState::Terminate ()
{
    Default[SHADE] = 0;
}
//----------------------------------------------------------------------------
ShadeState::ShadeState ()
{
    Shade = SM_SMOOTH;
}
//----------------------------------------------------------------------------
ShadeState::~ShadeState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ShadeState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Shade);

    WM3_END_DEBUG_STREAM_LOAD(ShadeState);
}
//----------------------------------------------------------------------------
void ShadeState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ShadeState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void ShadeState::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(Shade);

    WM3_END_DEBUG_STREAM_SAVE(ShadeState);
}
//----------------------------------------------------------------------------
StringTree* ShadeState::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    char* acString = 0;
    switch ( Shade )
    {
    case SM_FLAT:
        acString = StringTree::Format("mode = FLAT");
        break;
    case SM_SMOOTH:
        acString = StringTree::Format("mode = SMOOTH");
        break;
    default:  // SM_QUANTITY
        break;
    }
    pkTree->SetString(1,acString);

    // children
    pkTree->SetChild(0,GlobalState::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int ShadeState::GetMemoryUsed () const
{
    return sizeof(ShadeState) - sizeof(GlobalState) +
        GlobalState::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int ShadeState::GetDiskUsed () const
{
    return GlobalState::GetDiskUsed() +
        sizeof(Shade);
}
//----------------------------------------------------------------------------
