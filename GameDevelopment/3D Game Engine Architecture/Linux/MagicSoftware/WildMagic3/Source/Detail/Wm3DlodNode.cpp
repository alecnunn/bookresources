// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DlodNode.h"
#include "Wm3Camera.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,DlodNode,SwitchNode);
WM3_IMPLEMENT_STREAM(DlodNode);
WM3_IMPLEMENT_DEFAULT_NAME_ID(DlodNode,SwitchNode);

//----------------------------------------------------------------------------
DlodNode::DlodNode (int iQuantity, int iGrowBy)
    :
    SwitchNode(iQuantity,iGrowBy),
    m_kModelMinDist(iQuantity,iGrowBy),
    m_kModelMaxDist(iQuantity,iGrowBy),
    m_kWorldMinDist(iQuantity,iGrowBy),
    m_kWorldMaxDist(iQuantity,iGrowBy)
{
}
//----------------------------------------------------------------------------
void DlodNode::SetModelDistance (int i, float fMinDist, float fMaxDist)
{
    m_kModelMinDist.SetElement(i,fMinDist);
    m_kModelMaxDist.SetElement(i,fMaxDist);

    // In case the model distance arrays are resized, also set the world
    // distance arrays to cause the resize.
    m_kWorldMinDist.SetElement(i,fMinDist);
    m_kWorldMaxDist.SetElement(i,fMaxDist);
}
//----------------------------------------------------------------------------
void DlodNode::SelectLevelOfDetail (const Camera* pkCamera)
{
    // ASSERT:  The child array of an DlodNode is compacted--there are no
    // empty slots in the array and the number of children is GetQuantity().
    // Moreover, it is assumed that all model distance values were set for
    // these children.

    // compute world LOD center
    m_kWorldLodCenter = World.ApplyForward(m_kModelLodCenter);

    // compute world squared distance intervals
    int i;
    for (i = 0; i < m_kChild.GetQuantity(); i++)
    {
        float fValue = m_kModelMinDist[i];
        m_kWorldMinDist[i] = World.GetUniformScale()*fValue;

        fValue = m_kModelMaxDist[i];
        m_kWorldMaxDist[i] = World.GetUniformScale()*fValue;
    }

    // select the LOD child
    SetActiveChild(SN_INVALID_CHILD);
    if ( m_kChild.GetQuantity() > 0 )
    {
        Vector3f kDiff = m_kWorldLodCenter - pkCamera->GetWorldLocation();
        float fDist = kDiff.Length();

        for (i = 0; i < m_kChild.GetQuantity(); i++) 
        {
            if ( m_kWorldMinDist[i] <= fDist && fDist < m_kWorldMaxDist[i] )
            {
                SetActiveChild(i);
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------
void DlodNode::Draw (Renderer& rkRenderer, bool bNoCull)
{
    SelectLevelOfDetail(rkRenderer.GetCamera());
    SwitchNode::Draw(rkRenderer,bNoCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void DlodNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    SwitchNode::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kModelLodCenter);

    int i, iQuantity;
    rkStream.Read(iQuantity);
    m_kModelMinDist.SetMaxQuantity(iQuantity,false);
    for (i = 0; i < iQuantity; i++)
        rkStream.Read(m_kModelMinDist[i]);

    rkStream.Read(iQuantity);
    m_kModelMaxDist.SetMaxQuantity(iQuantity,false);
    for (i = 0; i < iQuantity; i++)
        rkStream.Read(m_kModelMaxDist[i]);

    WM3_END_DEBUG_STREAM_LOAD(DlodNode);
}
//----------------------------------------------------------------------------
void DlodNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    SwitchNode::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool DlodNode::Register (Stream& rkStream) const
{
    return SwitchNode::Register(rkStream);
}
//----------------------------------------------------------------------------
void DlodNode::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    SwitchNode::Save(rkStream);

    // native data
    rkStream.Write(m_kModelLodCenter);

    int i, iQuantity = m_kModelMinDist.GetMaxQuantity();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
        rkStream.Write(m_kModelMinDist[i]);

    iQuantity = m_kModelMaxDist.GetMaxQuantity();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
        rkStream.Write(m_kModelMaxDist[i]);

    // The world LOD center is computed from the model LOD center in
    // SelectLevelOfDetail.  The world distance extremes are also computed
    // from the model distance extremes in SelectLevelOfDetail.  These
    // world quantities do not need to be saved.

    WM3_END_DEBUG_STREAM_SAVE(DlodNode);
}
//----------------------------------------------------------------------------
StringTree* DlodNode::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(3,5);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("model lod center",
        m_kModelLodCenter));
    pkTree->SetString(2,StringTree::Format("world lod center",
        m_kWorldLodCenter));

    // children
    pkTree->SetChild(0,SwitchNode::SaveStrings());
    pkTree->SetChild(1,StringTree::Format("model min dist",
        m_kModelMinDist.GetQuantity(),m_kModelMinDist.GetArray()));
    pkTree->SetChild(2,StringTree::Format("model max dist",
        m_kModelMaxDist.GetQuantity(),m_kModelMaxDist.GetArray()));
    pkTree->SetChild(3,StringTree::Format("world min dist",
        m_kWorldMinDist.GetQuantity(),m_kWorldMinDist.GetArray()));
    pkTree->SetChild(4,StringTree::Format("world max dist",
        m_kWorldMaxDist.GetQuantity(),m_kWorldMaxDist.GetArray()));

    return pkTree;
}
//----------------------------------------------------------------------------
int DlodNode::GetMemoryUsed () const
{
    return sizeof(DlodNode) - sizeof(SwitchNode) +
        m_kModelMinDist.GetMaxQuantity()*sizeof(float) +
        m_kModelMaxDist.GetMaxQuantity()*sizeof(float) +
        m_kWorldMinDist.GetMaxQuantity()*sizeof(float) +
        m_kWorldMinDist.GetMaxQuantity()*sizeof(float);
}
//----------------------------------------------------------------------------
int DlodNode::GetDiskUsed () const
{
    return SwitchNode::GetDiskUsed() +
        sizeof(m_kModelLodCenter) +
        sizeof(int) + m_kModelMinDist.GetMaxQuantity()*sizeof(float) +
        sizeof(int) + m_kModelMaxDist.GetMaxQuantity()*sizeof(float);
}
//----------------------------------------------------------------------------
