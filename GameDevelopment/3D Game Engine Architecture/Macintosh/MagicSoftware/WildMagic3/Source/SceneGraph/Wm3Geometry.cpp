// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Geometry.h"
#include "Wm3Light.h"
#include "Wm3Texture.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Geometry,Spatial);
WM3_IMPLEMENT_ABSTRACT_STREAM(Geometry);

//----------------------------------------------------------------------------
Geometry::Geometry ()
    :
    ModelBound(BoundingVolume::Create()),
    Lights(8,8)
{
    memset(States,0,GlobalState::MAX_STATE*sizeof(GlobalState*));
}
//----------------------------------------------------------------------------
Geometry::Geometry (Vector3fArrayPtr spkVertices)
    :
    Vertices(spkVertices),
    ModelBound(BoundingVolume::Create()),
    Lights(8,8)
{
    memset(States,0,GlobalState::MAX_STATE*sizeof(GlobalState*));
    UpdateModelBound();
}
//----------------------------------------------------------------------------
Geometry::~Geometry ()
{
}
//----------------------------------------------------------------------------
void Geometry::UpdateMS (bool bUpdateNormals)
{
    UpdateModelBound();
    if ( bUpdateNormals )
        UpdateModelNormals();
}
//----------------------------------------------------------------------------
void Geometry::UpdateModelBound ()
{
    ModelBound->ComputeFromData(Vertices);
}
//----------------------------------------------------------------------------
void Geometry::UpdateWorldBound ()
{
    ModelBound->TransformBy(World,WorldBound);
}
//----------------------------------------------------------------------------
void Geometry::UpdateState (TStack<GlobalState*>* akGStack,
    TStack<Light*>* pkLStack)
{
    // update global state
    int i;
    for (i = 0; i < GlobalState::MAX_STATE; i++)
    {
        GlobalState* pkGState = 0;
        akGStack[i].GetTop(pkGState);
        assert( pkGState );
        States[i] = pkGState;
    }

    // update lights
    Light* const* akLight = pkLStack->GetData();
    int iQuantity = pkLStack->GetQuantity();
    Lights.RemoveAll();
    for (i = 0; i < iQuantity; i++)
        Lights.Append(akLight[i]);
}
//----------------------------------------------------------------------------
void Geometry::Draw (Renderer& rkRenderer, bool)
{
    rkRenderer.Draw(this);
}
//----------------------------------------------------------------------------
Geometry::PickRecord::PickRecord (Geometry* pkIObject, float fT)
    :
    Spatial::PickRecord(pkIObject,fT)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Geometry::GetObjectByName (const String& rkName)
{
    Object* pkFound = Spatial::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( Vertices )
    {
        pkFound = Vertices->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( Normals )
    {
        pkFound = Normals->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( ModelBound )
    {
        pkFound = ModelBound->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void Geometry::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Spatial::GetAllObjectsByName(rkName,rkObjects);

    if ( Vertices )
        Vertices->GetAllObjectsByName(rkName,rkObjects);

    if ( Normals )
        Normals->GetAllObjectsByName(rkName,rkObjects);

    if ( ModelBound )
        ModelBound->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* Geometry::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Spatial::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( Vertices )
    {
        pkFound = Vertices->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( Normals )
    {
        pkFound = Normals->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( ModelBound )
    {
        pkFound = ModelBound->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Geometry::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Spatial::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // Vertices
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // Normals
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // ModelBound
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // Indices
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(Geometry);
}
//----------------------------------------------------------------------------
void Geometry::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Spatial::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    Vertices = (Vector3fArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    Normals = (Vector3fArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    ModelBound = (BoundingVolume*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    Indices = (IntArray*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Geometry::Register (Stream& rkStream) const
{
    if ( !Spatial::Register(rkStream) )
        return false;

    if ( Vertices )
        Vertices->Register(rkStream);

    if ( Normals )
        Normals->Register(rkStream);

    if ( ModelBound )
        ModelBound->Register(rkStream);

    if ( Indices )
        Indices->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void Geometry::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Spatial::Save(rkStream);

    // link data
    rkStream.Write(Vertices);
    rkStream.Write(Normals);
    rkStream.Write(ModelBound);
    rkStream.Write(Indices);

    WM3_END_DEBUG_STREAM_SAVE(Geometry);
}
//----------------------------------------------------------------------------
StringTree* Geometry::SaveStrings (const char*)
{
    int iCQuantity = ( Normals ? 4 : 3 );
    StringTree* pkTree = new StringTree(4,iCQuantity);

    int iVQuantity = Vertices->GetQuantity();
    int iTQuantity = Indices->GetQuantity()/3;

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("model bound =",ModelBound));
    pkTree->SetString(2,StringTree::Format("vertex quantity =",iVQuantity));
    pkTree->SetString(3,StringTree::Format("triangle quantity =",iTQuantity));

    // children
    pkTree->SetChild(0,Spatial::SaveStrings());
    pkTree->SetChild(1,Vertices->SaveStrings("vertices"));
    pkTree->SetChild(2,Indices->SaveStrings("indices"));
    if ( Normals )
        pkTree->SetChild(3,Normals->SaveStrings("normals"));

    return pkTree;
}
//----------------------------------------------------------------------------
int Geometry::GetMemoryUsed () const
{
    return sizeof(Geometry) - sizeof(Spatial) +  Spatial::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int Geometry::GetDiskUsed () const
{
    return Spatial::GetDiskUsed() +
        sizeof(Vertices) +
        sizeof(Normals) +
        sizeof(ModelBound) +
        sizeof(Indices);
}
//----------------------------------------------------------------------------
