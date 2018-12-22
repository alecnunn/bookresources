// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3WireframeState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,WireframeState,GlobalState);
WM3_IMPLEMENT_STREAM(WireframeState);
WM3_IMPLEMENT_DEFAULT_NAME_ID(WireframeState,GlobalState);
WM3_IMPLEMENT_INITIALIZE(WireframeState);
WM3_IMPLEMENT_TERMINATE(WireframeState);

//----------------------------------------------------------------------------
void WireframeState::Initialize ()
{
    Default[WIREFRAME] = new WireframeState;
}
//----------------------------------------------------------------------------
void WireframeState::Terminate ()
{
    Default[WIREFRAME] = 0;
}
//----------------------------------------------------------------------------
WireframeState::WireframeState ()
{
    Enabled = false;
}
//----------------------------------------------------------------------------
WireframeState::~WireframeState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void WireframeState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Enabled);

    WM3_END_DEBUG_STREAM_LOAD(WireframeState);
}
//----------------------------------------------------------------------------
void WireframeState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool WireframeState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void WireframeState::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(Enabled);

    WM3_END_DEBUG_STREAM_SAVE(WireframeState);
}
//----------------------------------------------------------------------------
StringTree* WireframeState::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("enabled =",Enabled));

    // children
    pkTree->SetChild(0,GlobalState::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int WireframeState::GetMemoryUsed () const
{
    return sizeof(WireframeState) - sizeof(GlobalState) +
        GlobalState::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int WireframeState::GetDiskUsed () const
{
    return GlobalState::GetDiskUsed() +
        sizeof(char);  // Enabled
}
//----------------------------------------------------------------------------
