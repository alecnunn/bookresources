// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ShaderEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ShaderEffect,Effect);
WM3_IMPLEMENT_STREAM(ShaderEffect);

//----------------------------------------------------------------------------
ShaderEffect::ShaderEffect ()
{
    Draw = &Renderer::DrawShader;
}
//----------------------------------------------------------------------------
ShaderEffect::~ShaderEffect ()
{
}
//----------------------------------------------------------------------------
Effect* ShaderEffect::Clone ()
{
    ShaderEffect* pkClone = new ShaderEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void ShaderEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);

    ShaderEffect* pkDClone = (ShaderEffect*)pkClone;
    pkDClone->VShader = VShader;
    pkDClone->PShader = PShader;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* ShaderEffect::GetObjectByName (const String& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( VShader )
    {
        pkFound = VShader->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( PShader )
    {
        pkFound = PShader->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void ShaderEffect::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    if ( VShader )
        VShader->GetAllObjectsByName(rkName,rkObjects);

    if ( PShader )
        PShader->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* ShaderEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( VShader )
    {
        pkFound = VShader->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( PShader )
    {
        pkFound = PShader->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ShaderEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // VShader
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // PShader
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(ShaderEffect);
}
//----------------------------------------------------------------------------
void ShaderEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    VShader = (VertexShader*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    PShader = (PixelShader*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool ShaderEffect::Register (Stream& rkStream) const
{
    if ( !Effect::Register(rkStream) )
        return false;

    if ( VShader )
        VShader->Register(rkStream);

    if ( PShader )
        PShader->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void ShaderEffect::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // link data
    rkStream.Write(VShader);
    rkStream.Write(PShader);

    WM3_END_DEBUG_STREAM_SAVE(ShaderEffect);
}
//----------------------------------------------------------------------------
StringTree* ShaderEffect::SaveStrings (const char*)
{
    int iCQuantity = 1;
    if ( VShader )
        iCQuantity++;
    if ( PShader )
        iCQuantity++;

    StringTree* pkTree = new StringTree(1,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Effect::SaveStrings());
    iCQuantity = 1;
    if ( VShader )
        pkTree->SetChild(iCQuantity++,VShader->SaveStrings());
    if ( PShader )
        pkTree->SetChild(iCQuantity,PShader->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int ShaderEffect::GetMemoryUsed () const
{
    return sizeof(ShaderEffect) - sizeof(Effect) +  Effect::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int ShaderEffect::GetDiskUsed () const
{
    return Effect::GetDiskUsed() +
        sizeof(VShader) +
        sizeof(PShader);
}
//----------------------------------------------------------------------------
