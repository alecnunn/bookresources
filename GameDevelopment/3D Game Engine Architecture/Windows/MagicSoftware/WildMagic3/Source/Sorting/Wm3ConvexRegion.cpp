// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ConvexRegion.h"
#include "Wm3Portal.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ConvexRegion,Node);
WM3_IMPLEMENT_STREAM(ConvexRegion);

//----------------------------------------------------------------------------
ConvexRegion::ConvexRegion (int iPQuantity, Portal** apkPortal)
{
    m_iPQuantity = iPQuantity;
    m_apkPortal = apkPortal;
    m_bVisited = false;
}
//----------------------------------------------------------------------------
ConvexRegion::ConvexRegion ()
{
    m_iPQuantity = 0;
    m_apkPortal = 0;
    m_bVisited = false;
}
//----------------------------------------------------------------------------
ConvexRegion::~ConvexRegion ()
{
    for (int i = 0; i < m_iPQuantity; i++)
        delete m_apkPortal[i];

    delete[] m_apkPortal;
}
//----------------------------------------------------------------------------
void ConvexRegion::UpdateWorldData (double dAppTime)
{
    // update the region walls and contained objects
    Node::UpdateWorldData(dAppTime);

    // update the portal geometry
    for (int i = 0; i < m_iPQuantity; i++)
        m_apkPortal[i]->UpdateWorldData(World);
}
//----------------------------------------------------------------------------
void ConvexRegion::Draw (Renderer& rkRenderer, bool bNoCull)
{
    if ( !m_bVisited )
    {
        m_bVisited = true;

        // draw anything visible through open portals
        for (int i = 0; i < m_iPQuantity; i++)
            m_apkPortal[i]->Draw(rkRenderer);

        // draw the region walls and contained objects
        Node::Draw(rkRenderer,bNoCull);

        m_bVisited = false;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* ConvexRegion::GetObjectByName (const String& rkName)
{
    Object* pkFound = Node::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    for (int i = 0; i < m_iPQuantity; i++)
    {
        pkFound = m_apkPortal[i]->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void ConvexRegion::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Node::GetAllObjectsByName(rkName,rkObjects);

    for (int i = 0; i < m_iPQuantity; i++)
        m_apkPortal[i]->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* ConvexRegion::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Node::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    for (int i = 0; i < m_iPQuantity; i++)
    {
        pkFound = m_apkPortal[i]->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ConvexRegion::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // link data
    rkStream.Read(m_iPQuantity);
    if ( m_iPQuantity > 0 )
    {
        m_apkPortal = new Portal*[m_iPQuantity];
        for (int i = 0; i < m_iPQuantity; i++)
        {
            Object* pkPortal;
            rkStream.Read(pkPortal);
            pkLink->Add(pkPortal);
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(ConvexRegion);
}
//----------------------------------------------------------------------------
void ConvexRegion::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);

    for (int i = 0; i < m_iPQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_apkPortal[i] = (Portal*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool ConvexRegion::Register (Stream& rkStream) const
{
    if ( !Node::Register(rkStream) )
        return false;

    for (int i = 0; i < m_iPQuantity; i++)
        m_apkPortal[i]->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void ConvexRegion::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // link data
    rkStream.Write(m_iPQuantity);
    for (int i = 0; i < m_iPQuantity; i++)
        rkStream.Write(m_apkPortal[i]);

    WM3_END_DEBUG_STREAM_SAVE(ConvexRegion);
}
//----------------------------------------------------------------------------
StringTree* ConvexRegion::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,1+m_iPQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("portal quantity =",m_iPQuantity));

    // children
    pkTree->SetChild(0,Node::SaveStrings());
    for (int i = 0, j = 1; i < m_iPQuantity; i++, j++)
        pkTree->SetChild(j,m_apkPortal[i]->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int ConvexRegion::GetMemoryUsed () const
{
    int iSize = sizeof(ConvexRegion) - sizeof(Node) + Node::GetMemoryUsed();
    iSize += m_iPQuantity*sizeof(m_apkPortal[0]);
    return iSize;
}
//----------------------------------------------------------------------------
int ConvexRegion::GetDiskUsed () const
{
    return Node::GetDiskUsed() +
        sizeof(m_iPQuantity) +
        m_iPQuantity*sizeof(m_apkPortal[0]);
}
//----------------------------------------------------------------------------
