// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BspNode.h"
#include "Wm3Camera.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,BspNode,Node);
WM3_IMPLEMENT_STREAM(BspNode);
WM3_IMPLEMENT_DEFAULT_NAME_ID(BspNode,Node);

//----------------------------------------------------------------------------
BspNode::BspNode ()
    :
    Node(3,0),
    m_kModelPlane(Vector3f::ZERO,0.0f)
{
    m_kChild.Append(0);
    m_kChild.Append(0);
    m_kChild.Append(0);
}
//----------------------------------------------------------------------------
BspNode::BspNode (const Plane3f& rkModelPlane)
    :
    Node(3,0),
    m_kModelPlane(rkModelPlane)
{
    m_kChild.Append(0);
    m_kChild.Append(0);
    m_kChild.Append(0);
}
//----------------------------------------------------------------------------
BspNode::~BspNode ()
{
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::AttachPositiveChild (Spatial* pkChild)
{
    return SetChild(0,pkChild);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::AttachCoplanarChild (Spatial* pkChild)
{
    return SetChild(1,pkChild);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::AttachNegativeChild (Spatial* pkChild)
{
    return SetChild(2,pkChild);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::DetachPositiveChild ()
{
    return DetachChildAt(0);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::DetachCoplanarChild ()
{
    return DetachChildAt(1);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::DetachNegativeChild ()
{
    return DetachChildAt(2);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::GetPositiveChild ()
{
    return GetChild(0);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::GetCoplanarChild ()
{
    return GetChild(1);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::GetNegativeChild ()
{
    return GetChild(2);
}
//----------------------------------------------------------------------------
void BspNode::UpdateWorldData (double dAppTime)
{
    Node::UpdateWorldData(dAppTime);
    m_kWorldPlane = World.ApplyForward(m_kModelPlane);
}
//----------------------------------------------------------------------------
void BspNode::Draw (Renderer& rkRenderer, bool bNoCull)
{
    // draw children in back-to-front order
    SpatialPtr spkPChild = GetPositiveChild();
    SpatialPtr spkCChild = GetCoplanarChild();
    SpatialPtr spkNChild = GetNegativeChild();

    CameraPtr spkCamera = rkRenderer.GetCamera();
    int iLocSide = m_kWorldPlane.WhichSide(spkCamera->GetWorldLocation());
    int iFruSide = spkCamera->WhichSide(m_kWorldPlane);

    if ( iLocSide > 0 )
    {
        // camera origin on positive side of plane

        if ( iFruSide <= 0 )
        {
            // The frustum is on the negative side of the plane or straddles
            // the plane.  In either case, the negative child is potentially
            // visible.
            if ( spkNChild )
                spkNChild->Draw(rkRenderer,bNoCull);
        }

        if ( iFruSide == 0 )
        {
            // The frustum straddles the plane.  The coplanar child is
            // potentially visible.
            if ( spkCChild )
                spkCChild->Draw(rkRenderer,bNoCull);
        }

        if ( iFruSide >= 0 )
        {
            // The frustum is on the positive side of the plane or straddles
            // the plane.  In either case, the positive child is potentially
            // visible.
            if ( spkPChild )
                spkPChild->Draw(rkRenderer,bNoCull);
        }
    }
    else if ( iLocSide < 0 )
    {
        // camera origin on negative side of plane

        if ( iFruSide >= 0 )
        {
            // The frustum is on the positive side of the plane or straddles
            // the plane.  In either case, the positive child is potentially
            // visible.
            if ( spkPChild )
                spkPChild->Draw(rkRenderer,bNoCull);
        }

        if ( iFruSide == 0 )
        {
            // The frustum straddles the plane.  The coplanar child is
            // potentially visible.
            if ( spkCChild )
                spkCChild->Draw(rkRenderer,bNoCull);
        }

        if ( iFruSide <= 0 )
        {
            // The frustum is on the negative side of the plane or straddles
            // the plane.  In either case, the negative child is potentially
            // visible.
            if ( spkNChild )
                spkNChild->Draw(rkRenderer,bNoCull);
        }
    }
    else
    {
        // Camera origin on plane itself.  Both sides of the plane are
        // potentially visible as well as the plane itself.  Select the
        // first-to-be-drawn half space to be the one to which the camera
        // direction points.
        float fNdD = m_kWorldPlane.Normal.Dot(spkCamera->GetWorldDVector());
        if ( fNdD >= 0.0f )
        {
            if ( spkPChild )
                spkPChild->Draw(rkRenderer,bNoCull);

            if ( spkCChild )
                spkCChild->Draw(rkRenderer,bNoCull);

            if ( spkNChild )
                spkNChild->Draw(rkRenderer,bNoCull);
        }
        else
        {
            if ( spkNChild )
                spkNChild->Draw(rkRenderer,bNoCull);

            if ( spkCChild )
                spkCChild->Draw(rkRenderer,bNoCull);

            if ( spkPChild )
                spkPChild->Draw(rkRenderer,bNoCull);
        }
    }
}
//----------------------------------------------------------------------------
Spatial* BspNode::GetContainingNode (const Vector3f& rkPoint)
{
    SpatialPtr spkPChild = GetPositiveChild();
    SpatialPtr spkNChild = GetNegativeChild();

    if ( spkPChild || spkNChild )
    {
        BspNode* pkBspChild;

        if ( m_kWorldPlane.WhichSide(rkPoint) < 0 )
        {
            pkBspChild = DynamicCast<BspNode>(spkNChild);
            if ( pkBspChild )
                return pkBspChild->GetContainingNode(rkPoint);
            else
                return spkNChild;
        }
        else
        {
            pkBspChild = DynamicCast<BspNode>(spkPChild);
            if ( pkBspChild )
                return pkBspChild->GetContainingNode(rkPoint);
            else
                return spkPChild;
        }
    }

    return this;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BspNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kModelPlane);

    WM3_END_DEBUG_STREAM_LOAD(BspNode);
}
//----------------------------------------------------------------------------
void BspNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BspNode::Register (Stream& rkStream) const
{
    return Node::Register(rkStream);
}
//----------------------------------------------------------------------------
void BspNode::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // native data
    rkStream.Write(m_kModelPlane);

    // world plane is computed from model plane in update, no need to save
    WM3_END_DEBUG_STREAM_SAVE(BspNode);
}
//----------------------------------------------------------------------------
StringTree* BspNode::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("model plane =",m_kModelPlane));

    // children
    pkTree->SetChild(0,Node::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int BspNode::GetMemoryUsed () const
{
    return sizeof(BspNode) - sizeof(Node) + Node::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int BspNode::GetDiskUsed () const
{
    return Node::GetDiskUsed() + sizeof(m_kModelPlane);
}
//----------------------------------------------------------------------------
