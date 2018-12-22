// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BillboardNode.h"
#include "Wm3Camera.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,BillboardNode,Node);
WM3_IMPLEMENT_STREAM(BillboardNode);

//----------------------------------------------------------------------------
BillboardNode::BillboardNode (Camera* pkCamera, int iQuantity, int iGrowBy)
    :
    Node(iQuantity,iGrowBy),
    m_spkCamera(pkCamera)
{
}
//----------------------------------------------------------------------------
void BillboardNode::UpdateWorldData (double dAppTime)
{
    // Compute billboard's world transforms based on its parent's world
    // transform and its local transforms.  Notice that you should not call
    // Node::UpdateWorldData since that function updates its children.  The
    // children of a BillboardNode cannot be updated until the billboard is
    // aligned with the camera.
    Spatial::UpdateWorldData(dAppTime);

    if ( m_spkCamera )
    {
        // Inverse-transform the camera to the model space of the billboard.
        Vector3f kCLoc = World.ApplyInverse(m_spkCamera->GetWorldLocation());

        // To align the billboard, the projection of the camera to the
        // xz-plane of the billboard's model space determines the angle of
        // rotation about the billboard's model y-axis.  If the projected
        // camera is on the model axis (x = 0 and z = 0), ATan2 returns zero
        // (rather than NaN), so there is no need to trap this degenerate
        // case and handle it separately.
        float fAngle = Mathf::ATan2(kCLoc.X(),kCLoc.Z());
        Matrix3f kOrient(Vector3f::UNIT_Y,fAngle);
        World.SetRotate(World.GetRotate()*kOrient);
    }

    // update the children now that the billboard orientation is known
    for (int i = 0; i < m_kChild.GetQuantity(); i++)
    {
        Spatial* pkChild = m_kChild[i];
        if ( pkChild )
            pkChild->UpdateGS(dAppTime,false);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* BillboardNode::GetObjectByName (const String& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
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
void BillboardNode::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Node::GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkCamera )
        m_spkCamera->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* BillboardNode::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
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
void BillboardNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    WM3_END_DEBUG_STREAM_LOAD(BillboardNode);
}
//----------------------------------------------------------------------------
void BillboardNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BillboardNode::Register (Stream& rkStream) const
{
    return Node::Register(rkStream);
}
//----------------------------------------------------------------------------
void BillboardNode::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);
    // m_spkCamera is always set by the application, never streamed

    WM3_END_DEBUG_STREAM_SAVE(BillboardNode);
}
//----------------------------------------------------------------------------
StringTree* BillboardNode::SaveStrings (const char*)
{
    int iCQuantity = ( m_spkCamera ? 2 : 1 );
    StringTree* pkTree = new StringTree(1,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Node::SaveStrings());

    if ( m_spkCamera )
        pkTree->SetChild(1,m_spkCamera->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int BillboardNode::GetMemoryUsed () const
{
    return sizeof(BillboardNode) - sizeof(Node) + Node::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int BillboardNode::GetDiskUsed () const
{
    return Node::GetDiskUsed();
}
//----------------------------------------------------------------------------
