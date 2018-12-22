// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "SimplePatch.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,SimplePatch,SurfacePatch);
WM3_IMPLEMENT_STREAM(SimplePatch);

//----------------------------------------------------------------------------
SimplePatch::SimplePatch ()
    :
    SurfacePatch(-2.0f,2.0f,-2.0f,2.0f,true)
{
    Amplitude = 0.0f;
}
//----------------------------------------------------------------------------
SimplePatch::~SimplePatch ()
{
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::P (float fU, float fV) const
{
    // P(u,v) = (u,v,A*u*v)
    return Vector3f(fU,fV,Amplitude*fU*fV);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PU (float fU, float fV) const
{
    // P_u = (1,0,A*v)
    return Vector3f(1.0f,0.0f,Amplitude*fV);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PV (float fU, float fV) const
{
    // P_v = (0,1,A*u)
    return Vector3f(0.0f,1.0f,Amplitude*fU);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PUU (float fU, float fV) const
{
    // P_uu = (0,0,0)
    return Vector3f(0.0f,0.0f,0.0f);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PUV (float fU, float fV) const
{
    // P_uv = (0,0,A)
    return Vector3f(0.0f,0.0f,Amplitude);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PVV (float fU, float fV) const
{
    // P_vv = (0,0,0)
    return Vector3f(0.0f,0.0f,0.0f);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* SimplePatch::GetObjectByName (const String& rkName)
{
    return SurfacePatch::GetObjectByName(rkName);
}
//----------------------------------------------------------------------------
void SimplePatch::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    SurfacePatch::GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* SimplePatch::GetObjectByID (unsigned int uiID)
{
    return SurfacePatch::GetObjectByID(uiID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SimplePatch::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    SurfacePatch::Load(rkStream,pkLink);
    rkStream.Read(Amplitude);

    WM3_END_DEBUG_STREAM_LOAD(SimplePatch);
}
//----------------------------------------------------------------------------
void SimplePatch::Link (Stream& rkStream, Stream::Link* pkLink)
{
    SurfacePatch::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SimplePatch::Register (Stream& rkStream) const
{
    return SurfacePatch::Register(rkStream);
}
//----------------------------------------------------------------------------
void SimplePatch::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    SurfacePatch::Save(rkStream);
    rkStream.Write(Amplitude);

    WM3_END_DEBUG_STREAM_SAVE(SimplePatch);
}
//----------------------------------------------------------------------------
StringTree* SimplePatch::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("amplitude =",Amplitude));

    // children
    pkTree->SetChild(0,SurfacePatch::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int SimplePatch::GetMemoryUsed () const
{
    return sizeof(SimplePatch) - sizeof(SurfacePatch) +
        SurfacePatch::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int SimplePatch::GetDiskUsed () const
{
    return SurfacePatch::GetDiskUsed() +
        sizeof(Amplitude);
}
//----------------------------------------------------------------------------
