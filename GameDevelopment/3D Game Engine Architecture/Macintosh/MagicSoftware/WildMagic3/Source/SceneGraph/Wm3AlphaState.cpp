// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3AlphaState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,AlphaState,GlobalState);
WM3_IMPLEMENT_STREAM(AlphaState);
WM3_IMPLEMENT_DEFAULT_NAME_ID(AlphaState,GlobalState);
WM3_IMPLEMENT_INITIALIZE(AlphaState);
WM3_IMPLEMENT_TERMINATE(AlphaState);

//----------------------------------------------------------------------------
void AlphaState::Initialize ()
{
    Default[ALPHA] = new AlphaState;
}
//----------------------------------------------------------------------------
void AlphaState::Terminate ()
{
    Default[ALPHA] = 0;
}
//----------------------------------------------------------------------------
AlphaState::AlphaState ()
{
    BlendEnabled = false;
    SrcBlend = SBF_SRC_ALPHA;
    DstBlend = DBF_ONE_MINUS_SRC_ALPHA;
    TestEnabled = false;
    Test = TF_ALWAYS;
    Reference = 0.0f;
}
//----------------------------------------------------------------------------
AlphaState::~AlphaState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void AlphaState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    rkStream.Read(BlendEnabled);
    rkStream.Read(SrcBlend);
    rkStream.Read(DstBlend);
    rkStream.Read(TestEnabled);
    rkStream.Read(Test);
    rkStream.Read(Reference);

    WM3_END_DEBUG_STREAM_LOAD(AlphaState);
}
//----------------------------------------------------------------------------
void AlphaState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool AlphaState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void AlphaState::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(BlendEnabled);
    rkStream.Write(SrcBlend);
    rkStream.Write(DstBlend);
    rkStream.Write(TestEnabled);
    rkStream.Write(Test);
    rkStream.Write(Reference);

    WM3_END_DEBUG_STREAM_SAVE(AlphaState);
}
//----------------------------------------------------------------------------
StringTree* AlphaState::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(7,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("blend =",BlendEnabled));

    char* acString = 0;
    switch ( SrcBlend )
    {
    case SBF_ZERO:
        acString = StringTree::Format("src blend = ZERO");
        break;
    case SBF_ONE:
        acString = StringTree::Format("src blend = ONE");
        break;
    case SBF_DST_COLOR:
        acString = StringTree::Format("src blend = DST_COLOR");
        break;
    case SBF_ONE_MINUS_DST_COLOR:
        acString = StringTree::Format("src blend = ONE_MINUS_DST_COLOR");
        break;
    case SBF_SRC_ALPHA:
        acString = StringTree::Format("src blend = SRC_ALPHA");
        break;
    case SBF_ONE_MINUS_SRC_ALPHA:
        acString = StringTree::Format("src blend = ONE_MINUS_SRC_ALPHA");
        break;
    case SBF_DST_ALPHA:
        acString = StringTree::Format("src blend = DST_ALPHA");
        break;
    case SBF_ONE_MINUS_DST_ALPHA:
        acString = StringTree::Format("src blend = ONE_MINUS_DST_ALPHA");
        break;
    case SBF_SRC_ALPHA_SATURATE:
        acString = StringTree::Format("src blend = SRC_ALPHA_SATURATE");
        break;
    case SBF_CONSTANT_COLOR:
        acString = StringTree::Format("src blend = SBF_CONSTANT_COLOR");
        break;
    case SBF_ONE_MINUS_CONSTANT_COLOR:
        acString = StringTree::Format(
            "src blend = SBF_ONE_MINUS_CONSTANT_COLOR");
        break;
    case SBF_CONSTANT_ALPHA:
        acString = StringTree::Format("src blend = SBF_CONSTANT_ALPHA");
        break;
    case SBF_ONE_MINUS_CONSTANT_ALPHA:
        acString = StringTree::Format(
            "src blend = SBF_ONE_MINUS_CONSTANT_ALPHA");
        break;
    default:  // SBF_QUANTITY
        break;
    }
    pkTree->SetString(2,acString);

    acString = 0;
    switch ( DstBlend )
    {
    case DBF_ZERO:
        acString = StringTree::Format("dst blend = ZERO");
        break;
    case DBF_ONE:
        acString = StringTree::Format("dst blend = ONE");
        break;
    case DBF_SRC_COLOR:
        acString = StringTree::Format("dst blend = DST_COLOR");
        break;
    case DBF_ONE_MINUS_SRC_COLOR:
        acString = StringTree::Format("dst blend = ONE_MINUS_DST_COLOR");
        break;
    case DBF_SRC_ALPHA:
        acString = StringTree::Format("dst blend = SRC_ALPHA");
        break;
    case DBF_ONE_MINUS_SRC_ALPHA:
        acString = StringTree::Format("dst blend = ONE_MINUS_SRC_ALPHA");
        break;
    case DBF_DST_ALPHA:
        acString = StringTree::Format("dst blend = DST_ALPHA");
        break;
    case DBF_ONE_MINUS_DST_ALPHA:
        acString = StringTree::Format("dst blend = ONE_MINUS_DST_ALPHA");
        break;
    case DBF_CONSTANT_COLOR:
        acString = StringTree::Format("dst blend = DBF_CONSTANT_COLOR");
        break;
    case DBF_ONE_MINUS_CONSTANT_COLOR:
        acString = StringTree::Format(
            "dst blend = DBF_ONE_MINUS_CONSTANT_COLOR");
        break;
    case DBF_CONSTANT_ALPHA:
        acString = StringTree::Format("dst blend = DBF_CONSTANT_ALPHA");
        break;
    case DBF_ONE_MINUS_CONSTANT_ALPHA:
        acString = StringTree::Format(
            "dst blend = DBF_ONE_MINUS_CONSTANT_ALPHA");
        break;
    default:  // DBF_QUANTITY
        break;
    }
    pkTree->SetString(3,acString);

    pkTree->SetString(4,StringTree::Format("test =",TestEnabled));

    acString = 0;
    switch ( Test )
    {
    case TF_NEVER:
        acString = StringTree::Format("test func = NEVER");
        break;
    case TF_LESS:
        acString = StringTree::Format("test func = LESS");
        break;
    case TF_EQUAL:
        acString = StringTree::Format("test func = EQUAL");
        break;
    case TF_LEQUAL:
        acString = StringTree::Format("test func = LEQUAL");
        break;
    case TF_GREATER:
        acString = StringTree::Format("test func = GREATER");
        break;
    case TF_NOTEQUAL:
        acString = StringTree::Format("test func = NOTEQUAL");
        break;
    case TF_GEQUAL:
        acString = StringTree::Format("test func = GEQUAL");
        break;
    case TF_ALWAYS:
        acString = StringTree::Format("test func = ALWAYS");
        break;
    default:  // TF_QUANTITY
        break;
    }
    pkTree->SetString(5,acString);

    pkTree->SetString(6,StringTree::Format("test ref =",Reference));

    // children
    pkTree->SetChild(0,GlobalState::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int AlphaState::GetMemoryUsed () const
{
    return sizeof(AlphaState) - sizeof(GlobalState) +
        GlobalState::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int AlphaState::GetDiskUsed () const
{
    return GlobalState::GetDiskUsed() +
        sizeof(char) + // BlendEnabled
        sizeof(SrcBlend) +
        sizeof(DstBlend) +
        sizeof(char) + // TestEnabled
        sizeof(Test) +
        sizeof(Reference);
}
//----------------------------------------------------------------------------
