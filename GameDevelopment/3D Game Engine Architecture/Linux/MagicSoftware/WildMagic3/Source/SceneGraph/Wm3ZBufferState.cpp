// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ZBufferState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ZBufferState,GlobalState);
WM3_IMPLEMENT_STREAM(ZBufferState);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ZBufferState,GlobalState);
WM3_IMPLEMENT_INITIALIZE(ZBufferState);
WM3_IMPLEMENT_TERMINATE(ZBufferState);

//----------------------------------------------------------------------------
void ZBufferState::Initialize ()
{
    Default[ZBUFFER] = new ZBufferState;
}
//----------------------------------------------------------------------------
void ZBufferState::Terminate ()
{
    Default[ZBUFFER] = 0;
}
//----------------------------------------------------------------------------
ZBufferState::ZBufferState ()
{
    Enabled = true;
    Writable = true;
    Compare = ZBufferState::CF_LEQUAL;
}
//----------------------------------------------------------------------------
ZBufferState::~ZBufferState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ZBufferState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Enabled);
    rkStream.Read(Writable);
    rkStream.Read(Compare);

    WM3_END_DEBUG_STREAM_LOAD(ZBufferState);
}
//----------------------------------------------------------------------------
void ZBufferState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ZBufferState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void ZBufferState::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(Enabled);
    rkStream.Write(Writable);
    rkStream.Write(Compare);

    WM3_END_DEBUG_STREAM_SAVE(ZBufferState);
}
//----------------------------------------------------------------------------
StringTree* ZBufferState::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(4,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("enabled =",Enabled));
    pkTree->SetString(2,StringTree::Format("write =",Writable));

    char* acString = 0;
    switch ( Compare )
    {
    case CF_NEVER:
        acString = StringTree::Format("compare func = NEVER");
        break;
    case CF_LESS:
        acString = StringTree::Format("compare func = LESS");
        break;
    case CF_EQUAL:
        acString = StringTree::Format("compare func = EQUAL");
        break;
    case CF_LEQUAL:
        acString = StringTree::Format("compare func = LEQUAL");
        break;
    case CF_GREATER:
        acString = StringTree::Format("compare func = GREATER");
        break;
    case CF_NOTEQUAL:
        acString = StringTree::Format("compare func = NOTEQUAL");
        break;
    case CF_GEQUAL:
        acString = StringTree::Format("compare func = GEQUAL");
        break;
    case CF_ALWAYS:
        acString = StringTree::Format("compare func = ALWAYS");
        break;
    default:  // CF_QUANTITY
        break;
    }
    pkTree->SetString(3,acString);

    // children
    pkTree->SetChild(0,GlobalState::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int ZBufferState::GetMemoryUsed () const
{
    return sizeof(ZBufferState) - sizeof(GlobalState) +
        GlobalState::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int ZBufferState::GetDiskUsed () const
{
    return GlobalState::GetDiskUsed() +
        sizeof(char) + // Enabled
        sizeof(char) + // Writable
        sizeof(Compare);
}
//----------------------------------------------------------------------------
