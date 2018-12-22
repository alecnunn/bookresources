// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BoxBV.h"
#include "Wm3ContBox3.h"
#include "Wm3IntrRay3Box3.h"
#include "Wm3IntrBox3Box3.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,BoxBV,BoundingVolume);
WM3_IMPLEMENT_STREAM(BoxBV);
WM3_IMPLEMENT_DEFAULT_NAME_ID(BoxBV,BoundingVolume);

//----------------------------------------------------------------------------
BoxBV::BoxBV ()
    :
    m_kBox(Vector3f::ZERO,Vector3f::UNIT_X,Vector3f::UNIT_Y,Vector3f::UNIT_Z,
        1.0f,1.0f,1.0f)
{
}
//----------------------------------------------------------------------------
BoxBV::BoxBV (const Box3f& rkBox)
    :
    m_kBox(rkBox)
{
}
//----------------------------------------------------------------------------
void BoxBV::SetRadius (float fRadius)
{
    m_kBox.Extent[0] = fRadius;
    m_kBox.Extent[1] = fRadius;
    m_kBox.Extent[2] = fRadius;
}
//----------------------------------------------------------------------------
float BoxBV::GetRadius () const
{
    float fRadius = m_kBox.Extent[0];
    if ( fRadius < m_kBox.Extent[1] )
        fRadius = m_kBox.Extent[1];
    if ( fRadius < m_kBox.Extent[2] )
        fRadius = m_kBox.Extent[2];

    return fRadius;
}
//----------------------------------------------------------------------------
void BoxBV::ComputeFromData (const Vector3fArray* pkVertices)
{
    if ( pkVertices )
    {
        int iVQuantity = pkVertices->GetQuantity();
        const Vector3f* akVertex = pkVertices->GetData();
        m_kBox = ContOrientedBox(iVQuantity,akVertex);
    }
}
//----------------------------------------------------------------------------
void BoxBV::TransformBy (const Transformation& rkTransform,
    BoundingVolume* pkResult)
{
    Box3f& rkTarget = ((BoxBV*)pkResult)->m_kBox;
    rkTarget.Center = rkTransform.ApplyForward(m_kBox.Center);
    for (int i = 0; i < 3; i++)
    {
        rkTarget.Axis[i] = rkTransform.GetRotate()*m_kBox.Axis[i];
        rkTarget.Extent[i] = rkTransform.GetNorm()*m_kBox.Extent[i];
    }
}
//----------------------------------------------------------------------------
int BoxBV::WhichSide (const Plane3f& rkPlane) const
{
    float fProjCenter = rkPlane.Normal.Dot(m_kBox.Center) - rkPlane.Constant;
    float fAbs0 = Mathf::FAbs(rkPlane.Normal.Dot(m_kBox.Axis[0]));
    float fAbs1 = Mathf::FAbs(rkPlane.Normal.Dot(m_kBox.Axis[1]));
    float fAbs2 = Mathf::FAbs(rkPlane.Normal.Dot(m_kBox.Axis[2]));
    float fProjRadius = m_kBox.Extent[0]*fAbs0 + m_kBox.Extent[1]*fAbs1 +
        m_kBox.Extent[2]*fAbs2;

    if ( fProjCenter - fProjRadius >= 0.0f )
        return +1;

    if ( fProjCenter + fProjRadius <= 0.0f )
        return -1;

    return 0;
}
//----------------------------------------------------------------------------
bool BoxBV::TestIntersection (const Ray3f& rkRay) const
{
    return IntrRay3Box3<float>(rkRay,m_kBox).Test();
}
//----------------------------------------------------------------------------
bool BoxBV::TestIntersection (const BoundingVolume* pkInput) const
{
    return IntrBox3Box3<float>(m_kBox,((BoxBV*)pkInput)->m_kBox).Test();
}
//----------------------------------------------------------------------------
void BoxBV::CopyFrom (const BoundingVolume* pkInput)
{
    m_kBox = ((BoxBV*)pkInput)->m_kBox;
}
//----------------------------------------------------------------------------
void BoxBV::GrowToContain (const BoundingVolume* pkInput)
{
    m_kBox = MergeBoxes(m_kBox,((BoxBV*)pkInput)->m_kBox);
}
//----------------------------------------------------------------------------
bool BoxBV::Contains (const Vector3f& rkPoint) const
{
    return InBox(rkPoint,m_kBox);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BoxBV::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    BoundingVolume::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kBox.Center);
    rkStream.Read(m_kBox.Axis[0]);
    rkStream.Read(m_kBox.Axis[1]);
    rkStream.Read(m_kBox.Axis[2]);
    rkStream.Read(m_kBox.Extent[0]);
    rkStream.Read(m_kBox.Extent[1]);
    rkStream.Read(m_kBox.Extent[2]);

    WM3_END_DEBUG_STREAM_LOAD(BoxBV);
}
//----------------------------------------------------------------------------
void BoxBV::Link (Stream& rkStream, Stream::Link* pkLink)
{
    BoundingVolume::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BoxBV::Register (Stream& rkStream) const
{
    return BoundingVolume::Register(rkStream);
}
//----------------------------------------------------------------------------
void BoxBV::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    BoundingVolume::Save(rkStream);

    // native data
    rkStream.Write(m_kBox.Center);
    rkStream.Write(m_kBox.Axis[0]);
    rkStream.Write(m_kBox.Axis[1]);
    rkStream.Write(m_kBox.Axis[2]);
    rkStream.Write(m_kBox.Extent[0]);
    rkStream.Write(m_kBox.Extent[1]);
    rkStream.Write(m_kBox.Extent[2]);

    WM3_END_DEBUG_STREAM_SAVE(BoxBV);
}
//----------------------------------------------------------------------------
StringTree* BoxBV::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(8,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("center =",m_kBox.Center));
    pkTree->SetString(2,StringTree::Format("axis0 =",m_kBox.Axis[0]));
    pkTree->SetString(3,StringTree::Format("axis1 =",m_kBox.Axis[1]));
    pkTree->SetString(4,StringTree::Format("axis2 =",m_kBox.Axis[2]));
    pkTree->SetString(5,StringTree::Format("extent0 =",m_kBox.Extent[0]));
    pkTree->SetString(6,StringTree::Format("extent1 =",m_kBox.Extent[1]));
    pkTree->SetString(7,StringTree::Format("extent2 =",m_kBox.Extent[2]));

    // children
    pkTree->SetChild(0,BoundingVolume::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int BoxBV::GetMemoryUsed () const
{
    return sizeof(BoxBV) - sizeof(BoundingVolume) +
        BoundingVolume::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int BoxBV::GetDiskUsed () const
{
    return BoundingVolume::GetDiskUsed() +
        sizeof(m_kBox);
}
//----------------------------------------------------------------------------
