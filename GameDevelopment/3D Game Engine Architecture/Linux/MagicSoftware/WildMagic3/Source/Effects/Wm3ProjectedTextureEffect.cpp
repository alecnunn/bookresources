// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ProjectedTextureEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ProjectedTextureEffect,Effect);
WM3_IMPLEMENT_STREAM(ProjectedTextureEffect);

//----------------------------------------------------------------------------
ProjectedTextureEffect::ProjectedTextureEffect (Image* pkImage,
    Camera* pkCamera, int iApplyMode)
    :
    m_spkCamera(pkCamera)
{
    Draw = &Renderer::DrawProjectedTexture;

    Texture* pkTexture = new Texture;
    pkTexture->SetImage(pkImage);
    pkTexture->Apply = iApplyMode;
    pkTexture->Filter = Texture::FM_LINEAR;
    pkTexture->Mipmap = Texture::MM_LINEAR_LINEAR;
    pkTexture->Texgen = Texture::TG_PROJECTED_TEXTURE;
    Textures.Append(pkTexture);
    UVs.Append(0);
}
//----------------------------------------------------------------------------
ProjectedTextureEffect::ProjectedTextureEffect ()
{
    Draw = &Renderer::DrawProjectedTexture;
}
//----------------------------------------------------------------------------
ProjectedTextureEffect::~ProjectedTextureEffect ()
{
}
//----------------------------------------------------------------------------
Effect* ProjectedTextureEffect::Clone ()
{
    ProjectedTextureEffect* pkClone = new ProjectedTextureEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);

    ProjectedTextureEffect* pkDClone = (ProjectedTextureEffect*)pkClone;
    pkDClone->m_spkCamera = m_spkCamera;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* ProjectedTextureEffect::GetObjectByName (const String& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_spkCamera )
    {
        pkFound = m_spkCamera->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkCamera )
        m_spkCamera->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* ProjectedTextureEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_spkCamera )
    {
        pkFound = m_spkCamera->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ProjectedTextureEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkCamera
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(ProjectedTextureEffect);
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkCamera = (Camera*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool ProjectedTextureEffect::Register (Stream& rkStream) const
{
    if ( !Effect::Register(rkStream) )
        return false;

    if ( m_spkCamera )
        m_spkCamera->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // link data
    rkStream.Write(m_spkCamera);

    WM3_END_DEBUG_STREAM_SAVE(ProjectedTextureEffect);
}
//----------------------------------------------------------------------------
StringTree* ProjectedTextureEffect::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(1,2);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Effect::SaveStrings());
    pkTree->SetChild(1,m_spkCamera->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int ProjectedTextureEffect::GetMemoryUsed () const
{
    return sizeof(ProjectedTextureEffect) - sizeof(Effect) +
        Effect::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int ProjectedTextureEffect::GetDiskUsed () const
{
    return Effect::GetDiskUsed() +
        sizeof(m_spkCamera);
}
//----------------------------------------------------------------------------
