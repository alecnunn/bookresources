// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Camera.h"
#include "Wm3ConvexRegion.h"
#include "Wm3Portal.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Portal,Object);
WM3_IMPLEMENT_STREAM(Portal);

//----------------------------------------------------------------------------
Portal::Portal (int iVQuantity, Vector3f* akModelVertex,
    ConvexRegion* pkAdjacentRegion, bool bOpen)
{
    assert( iVQuantity >= 3 );

    m_iVQuantity = iVQuantity;
    m_akModelVertex = akModelVertex;
    m_akWorldVertex = new Vector3f[m_iVQuantity];
    m_pkAdjacentRegion = pkAdjacentRegion;
    m_bOpen = bOpen;
}
//----------------------------------------------------------------------------
Portal::Portal ()
{
    m_iVQuantity = 0;
    m_akModelVertex = 0;
    m_akWorldVertex = 0;
    m_pkAdjacentRegion = 0;
    m_bOpen = false;
}
//----------------------------------------------------------------------------
Portal::~Portal ()
{
    delete[] m_akModelVertex;
    delete[] m_akWorldVertex;
}
//----------------------------------------------------------------------------
void Portal::UpdateWorldData (const Transformation& rkWorld)
{
    rkWorld.ApplyForward(m_iVQuantity,m_akModelVertex,m_akWorldVertex);
}
//----------------------------------------------------------------------------
void Portal::Draw (Renderer& rkRenderer)
{
    // only draw adjacent regions if portal is open
    if ( !m_bOpen )
        return;

    // only draw visible portals
    Camera* pkCamera = rkRenderer.GetCamera();
    if ( pkCamera->Culled(m_iVQuantity,m_akWorldVertex,true) )
        return;

    // push world planes formed by camera eye point and portal edges
    int i0 = 0, i1 = m_iVQuantity - 1;
    for (/**/; i0 < m_iVQuantity; i1 = i0++)
    {
        Plane3f rkPlane(pkCamera->GetLocation(),m_akWorldVertex[i0],
            m_akWorldVertex[i1]);

        pkCamera->PushPlane(rkPlane);
    }

    // draw the adjacent region and any nonculled objects in it
    m_pkAdjacentRegion->Draw(rkRenderer);

    // pop world planes
    for (i0 = 0; i0 < m_iVQuantity; i0++)
        pkCamera->PopPlane();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Portal::GetObjectByName (const String& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_pkAdjacentRegion )
    {
        pkFound = m_pkAdjacentRegion->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void Portal::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    if ( m_pkAdjacentRegion )
         m_pkAdjacentRegion->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* Portal::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_pkAdjacentRegion )
    {
        pkFound = m_pkAdjacentRegion->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Portal::Load (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iVQuantity);
    if ( m_iVQuantity )
    {
        m_akModelVertex = new Vector3f[m_iVQuantity];
        m_akWorldVertex = new Vector3f[m_iVQuantity];
    }

    for (int i = 0; i < m_iVQuantity; i++)
        rkStream.Read(m_akModelVertex[i]);

    rkStream.Read(m_bOpen);

    // link data
    Object* pkAdjacentRegion;
    rkStream.Read(pkAdjacentRegion);
    pkLink->Add(pkAdjacentRegion);
}
//----------------------------------------------------------------------------
void Portal::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_pkAdjacentRegion = (ConvexRegion*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Portal::Register (Stream& rkStream) const
{
    if ( !Object::Register(rkStream) )
        return false;

    if ( m_pkAdjacentRegion )
        m_pkAdjacentRegion->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void Portal::Save (Stream& rkStream) const
{
    Object::Save(rkStream);

    // Native data (world vertices are computed form model vertices in the
    // update call, no need to save)
    rkStream.Write(m_iVQuantity);
    for (int i = 0; i < m_iVQuantity; i++)
        rkStream.Write(m_akModelVertex[i]);

    rkStream.Write(m_bOpen);

    // link data
    rkStream.Write(m_pkAdjacentRegion);
}
//----------------------------------------------------------------------------
StringTree* Portal::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(4+2*m_iVQuantity,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("open =",m_bOpen));
    pkTree->SetString(2,StringTree::Format("adjacent =",m_pkAdjacentRegion));
    pkTree->SetString(3,StringTree::Format("vertex quantity =",m_iVQuantity));

    char acPrefix[64];
    int i, j = 4;
    for (i = 0; i < m_iVQuantity; i++, j++)
    {
        sprintf(acPrefix,"model[%d] =",i);
        pkTree->SetString(j,StringTree::Format(acPrefix,m_akModelVertex[i]));
    }
    for (i = 0; i < m_iVQuantity; i++, j++)
    {
        sprintf(acPrefix,"world[%d] =",i);
        pkTree->SetString(j,StringTree::Format(acPrefix,m_akWorldVertex[i]));
    }

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Portal::GetMemoryUsed () const
{
    int iSize = sizeof(Portal) - sizeof(Object) + Object::GetMemoryUsed();
    iSize += m_iVQuantity*(sizeof(m_akModelVertex[0]));
    iSize += m_iVQuantity*(sizeof(m_akWorldVertex[0]));
    return iSize;
}
//----------------------------------------------------------------------------
int Portal::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        sizeof(m_iVQuantity) +
        m_iVQuantity*sizeof(m_akModelVertex[0]) +
        sizeof(char) + // m_bOpen
        sizeof(m_pkAdjacentRegion);
}
//----------------------------------------------------------------------------
