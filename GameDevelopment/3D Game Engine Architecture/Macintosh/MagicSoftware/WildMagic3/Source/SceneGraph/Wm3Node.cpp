// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Node.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Node,Spatial);
WM3_IMPLEMENT_STREAM(Node);

//----------------------------------------------------------------------------
Node::Node (int iQuantity, int iGrowBy)
    :
    m_kChild(iQuantity,iGrowBy)
{
    m_iUsed = 0;
}
//----------------------------------------------------------------------------
Node::~Node ()
{
    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        SpatialPtr spkChild = DetachChildAt(i);
        spkChild = 0;
    }
}
//----------------------------------------------------------------------------
int Node::AttachChild (Spatial* pkChild)
{
    // Some folks are of the impression that a node can have multiple parents,
    // the scene graph therefore being a DAG.  That is not the case.  The
    // parent-child relationships form a tree.  This assertion is to let folks
    // know this and to warn them that a child is being kidnapped from another
    // parent.  To be safe, you should really call DetachChild before you
    // reattach somewhere else with AttachChild or SetChild.
    assert( pkChild && !pkChild->GetParent() );

    pkChild->SetParent(this);
    m_iUsed++;

    // attach child in first available slot (if any)
    int iQuantity = m_kChild.GetQuantity();
    for (int i = 0; i < iQuantity; i++)
    {
        if ( m_kChild[i] == 0 )
        {
            m_kChild[i] = pkChild;
            return i;
        }
    }

    // all slots used, increase array size
    m_kChild.Append(pkChild);
    return iQuantity;
}
//----------------------------------------------------------------------------
int Node::DetachChild (Spatial* pkChild)
{
    if ( pkChild )
    {
        // search to see if child exists
        for (int i = 0; i < m_kChild.GetQuantity(); i++)
        {
            if ( m_kChild[i] == pkChild )
            {
                // child found, detach it
                pkChild->SetParent(0);
                m_kChild[i] = 0;
                m_iUsed--;
                return i;
            }
        }
    }

    return -1;
}
//----------------------------------------------------------------------------
SpatialPtr Node::DetachChildAt (int i)
{
    if ( i < m_kChild.GetQuantity() )
    {
        SpatialPtr spkChild = m_kChild[i];
        if ( spkChild )
        {
            // child exists in slot, detach it
            spkChild->SetParent(0);
            m_kChild[i] = 0;
            m_iUsed--;
        }
        return spkChild;
    }
    return 0;
}
//----------------------------------------------------------------------------
SpatialPtr Node::SetChild (int i, Spatial* pkChild)
{
    // Some folks are of the impression that a node can have multiple parents,
    // the scene graph therefore being a DAG.  That is not the case.  The
    // parent-child relationships form a tree.  This assertion is to let folks
    // know this and to warn them that a child is being kidnapped from another
    // parent.  To be safe, you should really call DetachChild before you
    // reattach somewhere else with AttachChild or SetChild.
    if ( pkChild )
    {
        assert( !pkChild->GetParent() );
    }

    if ( i < m_kChild.GetQuantity() )
    {
        // detach child currently in slot
        Spatial* pkPreviousChild = m_kChild[i];
        if ( pkPreviousChild )
        {
            pkPreviousChild->SetParent(0);
            m_iUsed--;
        }

        // attach new child to slot
        if ( pkChild )
        {
            pkChild->SetParent(this);
            m_iUsed++;
        }

        m_kChild[i] = pkChild;
        return pkPreviousChild;
    }

    // index out of range, increase array size and attach new child
    pkChild->SetParent(this);
    m_kChild.SetElement(i,pkChild);
    m_iUsed++;
    return 0;
}
//----------------------------------------------------------------------------
SpatialPtr Node::GetChild (int i)
{
    return i < m_kChild.GetQuantity() ? m_kChild[i] : 0;
}
//----------------------------------------------------------------------------
void Node::UpdateWorldData (double dAppTime)
{
    Spatial::UpdateWorldData(dAppTime);

    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        Spatial* pkChild = m_kChild[i];
        if ( pkChild )
            pkChild->UpdateGS(dAppTime,false);
    }
}
//----------------------------------------------------------------------------
void Node::UpdateWorldBound ()
{
    if ( !WorldBoundIsCurrent )
    {
        bool bFoundFirstBound = false;
        for (int i = 0; i < m_kChild.GetQuantity(); i++)
        {
            Spatial* pkChild = m_kChild[i];
            if ( pkChild )
            {
                if ( bFoundFirstBound )
                {
                    // merge current world bound with child world bound
                    WorldBound->GrowToContain(pkChild->WorldBound);
                }
                else
                {
                    // set world bound to first non-null child world bound
                    bFoundFirstBound = true;
                    WorldBound->CopyFrom(pkChild->WorldBound);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void Node::UpdateState (TStack<GlobalState*>* akGStack,
    TStack<Light*>* pkLStack)
{
    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        Spatial* pkChild = m_kChild[i];
        if ( pkChild )
            pkChild->UpdateRS(akGStack,pkLStack);
    }
}
//----------------------------------------------------------------------------
void Node::Draw (Renderer& rkRenderer, bool bNoCull)
{
    if ( m_spkEffect == 0 )
    {
        for (int i = 0; i < m_kChild.GetQuantity(); i++)
        {
            Spatial* pkChild = m_kChild[i];
            if ( pkChild )
                pkChild->OnDraw(rkRenderer,bNoCull);
        }
    }
    else
    {
        // A "global" effect might require multipass rendering, so the Node
        // must be passed to the renderer for special handling.
        rkRenderer.Draw(this);
    }
}
//----------------------------------------------------------------------------
void Node::DoPick (const Ray3f& rkRay, PickArray& rkResults)
{
    if ( WorldBound->TestIntersection(rkRay) )
    {
        for (int i = 0; i < m_kChild.GetQuantity(); i++)
        {
            Spatial* pkChild = m_kChild[i];
            if ( pkChild )
                pkChild->DoPick(rkRay,rkResults);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Node::GetObjectByName (const String& rkName)
{
    Object* pkFound = Spatial::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        Spatial* pkChild = m_kChild[i];
        if ( pkChild )
        {
            pkFound = pkChild->GetObjectByName(rkName);
            if ( pkFound )
                return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void Node::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Spatial::GetAllObjectsByName(rkName,rkObjects);

    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        Spatial* pkChild = m_kChild[i];
        if ( pkChild )
            pkChild->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* Node::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Spatial::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        Spatial* pkChild = m_kChild[i];
        if ( pkChild )
        {
            pkFound = pkChild->GetObjectByID(uiID);
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
void Node::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Spatial::Load(rkStream,pkLink);

    // link data
    int iMaxQuantity, iGrowBy, iQuantity;
    rkStream.Read(iMaxQuantity);
    rkStream.Read(iGrowBy);
    rkStream.Read(iQuantity);
    m_kChild.SetMaxQuantity(iMaxQuantity,false);
    m_kChild.SetGrowBy(iGrowBy);
    for (int i = 0; i < iMaxQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);
        pkLink->Add(pkObject);
    }

    WM3_END_DEBUG_STREAM_LOAD(Node);
}
//----------------------------------------------------------------------------
void Node::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Spatial::Link(rkStream,pkLink);

    for (int i = 0; i < m_kChild.GetMaxQuantity(); i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        Spatial* pkChild = (Spatial*)rkStream.GetFromMap(pkLinkID);
        if ( pkChild )
            SetChild(i,pkChild);
    }
}
//----------------------------------------------------------------------------
bool Node::Register (Stream& rkStream) const
{
    if ( !Spatial::Register(rkStream) )
        return false;

    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        if ( m_kChild[i] )
            m_kChild[i]->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void Node::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Spatial::Save(rkStream);

    // link data
    rkStream.Write(m_kChild.GetMaxQuantity());
    rkStream.Write(m_kChild.GetGrowBy());
    rkStream.Write(m_kChild.GetQuantity());
    const SpatialPtr* pkArray = m_kChild.GetArray();
    for (int i = 0; i < m_kChild.GetMaxQuantity(); i++)
        rkStream.Write(pkArray[i]);

    WM3_END_DEBUG_STREAM_SAVE(Node);
}
//----------------------------------------------------------------------------
StringTree* Node::SaveStrings (const char*)
{
    int iCQuantity = m_kChild.GetQuantity();
    StringTree* pkTree = new StringTree(3,iCQuantity+1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("quantity =",iCQuantity));
    pkTree->SetString(2,StringTree::Format("used =",m_iUsed));

    // children
    pkTree->SetChild(0,Spatial::SaveStrings());
    for (int i = 0; i < iCQuantity; i++)
    {
        Spatial* pkChild = m_kChild[i];
        if ( pkChild )
        {
            pkTree->SetChild(i+1,pkChild->SaveStrings());
        }
        else
        {
            StringTree* pkEmpty = new StringTree(1,0);
            pkEmpty->SetString(0,StringTree::Format("unused slot"));
            pkTree->SetChild(i+1,pkEmpty);
        }
    }

    return pkTree;
}
//----------------------------------------------------------------------------
int Node::GetMemoryUsed () const
{
    return sizeof(Node) - sizeof(Spatial) + Spatial::GetMemoryUsed() +
        m_kChild.GetMaxQuantity()*sizeof(m_kChild[0]);
}
//----------------------------------------------------------------------------
int Node::GetDiskUsed () const
{
    return Spatial::GetDiskUsed() +
        3*sizeof(int) +  // m_kChild maxquantity, growby, quantity
        m_kChild.GetMaxQuantity()*sizeof(m_kChild[0]);
}
//----------------------------------------------------------------------------
