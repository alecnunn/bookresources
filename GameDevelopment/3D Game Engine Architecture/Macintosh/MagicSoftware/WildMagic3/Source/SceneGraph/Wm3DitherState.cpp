// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DitherState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,DitherState,GlobalState);
WM3_IMPLEMENT_STREAM(DitherState);
WM3_IMPLEMENT_DEFAULT_NAME_ID(DitherState,GlobalState);
WM3_IMPLEMENT_INITIALIZE(DitherState);
WM3_IMPLEMENT_TERMINATE(DitherState);

//----------------------------------------------------------------------------
void DitherState::Initialize ()
{
    Default[DITHER] = new DitherState;
}
//----------------------------------------------------------------------------
void DitherState::Terminate ()
{
    Default[DITHER] = 0;
}
//----------------------------------------------------------------------------
DitherState::DitherState ()
{
    Enabled = false;
}
//----------------------------------------------------------------------------
DitherState::~DitherState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void DitherState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Enabled);

    WM3_END_DEBUG_STREAM_LOAD(DitherState);
}
//----------------------------------------------------------------------------
void DitherState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool DitherState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void DitherState::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(Enabled);

    WM3_END_DEBUG_STREAM_SAVE(DitherState);
}
//----------------------------------------------------------------------------
StringTree* DitherState::SaveStrings (const char*)
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
int DitherState::GetMemoryUsed () const
{
    return sizeof(DitherState) - sizeof(GlobalState) +
        GlobalState::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int DitherState::GetDiskUsed () const
{
    return GlobalState::GetDiskUsed() +
        sizeof(char); // Enabled
}
//----------------------------------------------------------------------------
