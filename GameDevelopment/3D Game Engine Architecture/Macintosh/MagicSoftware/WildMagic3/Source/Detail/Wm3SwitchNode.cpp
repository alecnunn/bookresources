// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3SwitchNode.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,SwitchNode,Node);
WM3_IMPLEMENT_STREAM(SwitchNode);
WM3_IMPLEMENT_DEFAULT_NAME_ID(SwitchNode,Node);

//----------------------------------------------------------------------------
SwitchNode::SwitchNode (int iQuantity, int iGrowBy)
    :
    Node(iQuantity,iGrowBy)
{
    m_iActiveChild = SN_INVALID_CHILD;
}
//----------------------------------------------------------------------------
void SwitchNode::DoPick (const Ray3f& rkRay, PickArray& rkResults)
{
    if ( m_iActiveChild != SN_INVALID_CHILD )
    {
        if ( WorldBound->TestIntersection(rkRay) )
        {
            Spatial* pkChild = m_kChild[m_iActiveChild];
            if ( pkChild )
                pkChild->DoPick(rkRay,rkResults);
        }
    }
}
//----------------------------------------------------------------------------
void SwitchNode::Draw (Renderer& rkRenderer, bool bNoCull)
{
    if ( m_iActiveChild != SN_INVALID_CHILD )
    {
        Spatial* pkChild = m_kChild[m_iActiveChild];
        if ( pkChild )
            pkChild->OnDraw(rkRenderer,bNoCull);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SwitchNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iActiveChild);

    WM3_END_DEBUG_STREAM_LOAD(SwitchNode);
}
//----------------------------------------------------------------------------
void SwitchNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SwitchNode::Register (Stream& rkStream) const
{
    return Node::Register(rkStream);
}
//----------------------------------------------------------------------------
void SwitchNode::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // native data
    rkStream.Write(m_iActiveChild);

    WM3_END_DEBUG_STREAM_SAVE(SwitchNode);
}
//----------------------------------------------------------------------------
StringTree* SwitchNode::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("active",m_iActiveChild));

    // children
    pkTree->SetChild(0,Node::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int SwitchNode::GetMemoryUsed () const
{
    return sizeof(SwitchNode) - sizeof(Node) + Node::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int SwitchNode::GetDiskUsed () const
{
    return Node::GetDiskUsed() +
		 sizeof(m_iActiveChild);
}
//----------------------------------------------------------------------------
