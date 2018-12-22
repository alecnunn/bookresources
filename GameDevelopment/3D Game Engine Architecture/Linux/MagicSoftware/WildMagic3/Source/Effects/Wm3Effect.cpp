// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Effect.h"
#include "Wm3Light.h"
#include "Wm3Texture.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Effect,Object);
WM3_IMPLEMENT_STREAM(Effect);

//----------------------------------------------------------------------------
Effect::Effect ()
    :
    Textures(8,8),
    UVs(8,8)
{
    Draw = &Renderer::DrawPrimitive;
}
//----------------------------------------------------------------------------
Effect::~Effect ()
{
}
//----------------------------------------------------------------------------
Effect* Effect::Clone ()
{
    Effect* pkClone = new Effect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void Effect::DoCloning (Effect* pkClone)
{
    pkClone->Draw = Draw;
    pkClone->ColorRGBs = ColorRGBs;
    pkClone->ColorRGBAs = ColorRGBAs;

    int i;
    for ( i = 0; i < Textures.GetQuantity(); i++)
        pkClone->Textures.Append(Textures[i]);

    for ( i = 0; i < UVs.GetQuantity(); i++)
        pkClone->UVs.Append(UVs[i]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Effect::GetObjectByName (const String& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( ColorRGBs )
    {
        pkFound = ColorRGBs->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( ColorRGBAs )
    {
        pkFound = ColorRGBAs->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    int i;
    for (i = 0; i < Textures.GetQuantity(); i++)
    {
        if ( Textures[i] )
        {
            pkFound = Textures[i]->GetObjectByName(rkName);
            if ( pkFound )
                return pkFound;
        }
    }

    for (i = 0; i < UVs.GetQuantity(); i++)
    {
        if ( UVs[i] )
        {
            pkFound = UVs[i]->GetObjectByName(rkName);
            if ( pkFound )
                return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void Effect::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    if ( ColorRGBs )
        ColorRGBs->GetAllObjectsByName(rkName,rkObjects);

    if ( ColorRGBAs )
        ColorRGBAs->GetAllObjectsByName(rkName,rkObjects);

    int i;
    for (i = 0; i < Textures.GetQuantity(); i++)
    {
        if ( Textures[i] )
            Textures[i]->GetAllObjectsByName(rkName,rkObjects);
    }

    for (i = 0; i < UVs.GetQuantity(); i++)
    {
        if ( UVs[i] )
            UVs[i]->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* Effect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( ColorRGBs )
    {
        pkFound = ColorRGBs->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( ColorRGBAs )
    {
        pkFound = ColorRGBAs->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    int i;
    for (i = 0; i < Textures.GetQuantity(); i++)
    {
        if ( Textures[i] )
        {
            pkFound = Textures[i]->GetObjectByID(uiID);
            if ( pkFound )
                return pkFound;
        }
    }

    for (i = 0; i < UVs.GetQuantity(); i++)
    {
        if ( UVs[i] )
        {
            pkFound = UVs[i]->GetObjectByID(uiID);
            if ( pkFound )
                return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Effect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // ColorRGBs
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // ColorRGBAs
    pkLink->Add(pkObject);

    int iQuantity, i;
    rkStream.Read(iQuantity);
    if ( iQuantity > 0 )
    {
        for (i = 0; i < iQuantity; i++)
        {
            rkStream.Read(pkObject);  // Textures[i]
            pkLink->Add(pkObject);
            Textures.Append(0);  // to force increment of quantity
        }
    }

    rkStream.Read(iQuantity);
    if ( iQuantity > 0 )
    {
        for (i = 0; i < iQuantity; i++)
        {
            rkStream.Read(pkObject);  // UVs[i]
            pkLink->Add(pkObject);
            UVs.Append(0);  // to force increment of quantity
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(Effect);
}
//----------------------------------------------------------------------------
void Effect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    ColorRGBs = (ColorRGBArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    ColorRGBAs = (ColorRGBAArray*)rkStream.GetFromMap(pkLinkID);

    int i;
    for (i = 0; i < Textures.GetQuantity(); i++)
    {
        pkLinkID = pkLink->GetLinkID();
        Textures[i] = (Texture*)rkStream.GetFromMap(pkLinkID);
    }

    for (i = 0; i < UVs.GetQuantity(); i++)
    {
        pkLinkID = pkLink->GetLinkID();
        UVs[i] = (Vector2fArray*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool Effect::Register (Stream& rkStream) const
{
    if ( !Object::Register(rkStream) )
        return false;

    if ( ColorRGBs )
        ColorRGBs->Register(rkStream);

    if ( ColorRGBAs )
        ColorRGBAs->Register(rkStream);

    int i;
    for (i = 0; i < Textures.GetQuantity(); i++)
    {
        if ( Textures[i] )
            Textures[i]->Register(rkStream);
    }

    for (i = 0; i < UVs.GetQuantity(); i++)
    {
        if ( UVs[i] )
            UVs[i]->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void Effect::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // link data
    rkStream.Write(ColorRGBs);
    rkStream.Write(ColorRGBAs);

    int i, iQuantity = Textures.GetQuantity();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
        rkStream.Write(Textures[i]);

    iQuantity = UVs.GetQuantity();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
        rkStream.Write(UVs[i]);

    WM3_END_DEBUG_STREAM_SAVE(Effect);
}
//----------------------------------------------------------------------------
StringTree* Effect::SaveStrings (const char*)
{
    int iCQuantity = Textures.GetQuantity() + UVs.GetQuantity() + 1;
    StringTree* pkTree = new StringTree(1,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    StringTree* pkEmpty;
    int i;

    for (i = 0; i < Textures.GetQuantity(); i++)
    {
        Texture* pkTexture = Textures[i];
        if ( pkTexture )
        {
            pkTree->SetChild(i+1,pkTexture->SaveStrings());
        }
        else
        {
            pkEmpty = new StringTree(1,0);
            pkEmpty->SetString(0,StringTree::Format("unused slot"));
            pkTree->SetChild(i+1,pkEmpty);
        }
    }

    for (i = 0; i < UVs.GetQuantity(); i++)
    {
        Vector2fArray* pkUV = UVs[i];
        if ( pkUV )
        {
            pkTree->SetChild(i+1,pkUV->SaveStrings());
        }
        else
        {
            pkEmpty = new StringTree(1,0);
            pkEmpty->SetString(0,StringTree::Format("unused slot"));
            pkTree->SetChild(i+1,pkEmpty);
        }
    }

    return pkTree;
}
//----------------------------------------------------------------------------
int Effect::GetMemoryUsed () const
{
    return sizeof(Effect) - sizeof(Object) +  Object::GetMemoryUsed() +
        Textures.GetMaxQuantity()*sizeof(TexturePtr) +
        UVs.GetMaxQuantity()*sizeof(Vector2fArrayPtr);
}
//----------------------------------------------------------------------------
int Effect::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        sizeof(ColorRGBs) +
        sizeof(ColorRGBAs) +
        sizeof(int) + Textures.GetQuantity()*sizeof(Textures[0]) +
        sizeof(int) + UVs.GetQuantity()*sizeof(UVs[0]);
}
//----------------------------------------------------------------------------
