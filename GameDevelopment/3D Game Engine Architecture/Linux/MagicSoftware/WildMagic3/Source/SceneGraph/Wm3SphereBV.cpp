// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3SphereBV.h"
#include "Wm3ContSphere3.h"
#include "Wm3IntrRay3Sphere3.h"
#include "Wm3IntrSphere3Sphere3.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,SphereBV,BoundingVolume);
WM3_IMPLEMENT_STREAM(SphereBV);
WM3_IMPLEMENT_DEFAULT_NAME_ID(SphereBV,BoundingVolume);

//----------------------------------------------------------------------------
// The factory function of BoundingVolume creates SphereBV objects.  You may
// delete this and allow another bounding volume class to be the default.
//----------------------------------------------------------------------------
BoundingVolume* BoundingVolume::Create ()
{
    return new SphereBV;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
SphereBV::SphereBV ()
    :
    m_kSphere(Vector3f::ZERO,0.0f)
{
}
//----------------------------------------------------------------------------
SphereBV::SphereBV (const Sphere3f& rkSphere)
    :
    m_kSphere(rkSphere)
{
}
//----------------------------------------------------------------------------
void SphereBV::ComputeFromData (const Vector3fArray* pkVertices)
{
    if ( pkVertices )
    {
        int iVQuantity = pkVertices->GetQuantity();
        const Vector3f* akVertex = pkVertices->GetData();
        m_kSphere = ContSphereAverage(iVQuantity,akVertex);
    }
}
//----------------------------------------------------------------------------
void SphereBV::TransformBy (const Transformation& rkTransform,
    BoundingVolume* pkResult)
{
    Sphere3f& rkTarget = ((SphereBV*)pkResult)->m_kSphere;
    rkTarget.Center = rkTransform.ApplyForward(m_kSphere.Center);
    rkTarget.Radius = rkTransform.GetNorm()*m_kSphere.Radius;
}
//----------------------------------------------------------------------------
int SphereBV::WhichSide (const Plane3f& rkPlane) const
{
    float fDistance = rkPlane.DistanceTo(m_kSphere.Center);

    if ( fDistance <= -m_kSphere.Radius )
        return -1;
    else if ( fDistance >= m_kSphere.Radius )
        return +1;

    return 0;
}
//----------------------------------------------------------------------------
bool SphereBV::TestIntersection (const Ray3f& rkRay) const
{
    return IntrRay3Sphere3<float>(rkRay,m_kSphere).Test();
}
//----------------------------------------------------------------------------
bool SphereBV::TestIntersection (const BoundingVolume* pkInput) const
{
    return IntrSphere3Sphere3<float>(m_kSphere,
        ((SphereBV*)pkInput)->m_kSphere).Test();
}
//----------------------------------------------------------------------------
void SphereBV::CopyFrom (const BoundingVolume* pkInput)
{
    m_kSphere = ((SphereBV*)pkInput)->m_kSphere;
}
//----------------------------------------------------------------------------
void SphereBV::GrowToContain (const BoundingVolume* pkInput)
{
    m_kSphere = MergeSpheres(m_kSphere,((SphereBV*)pkInput)->m_kSphere);
}
//----------------------------------------------------------------------------
bool SphereBV::Contains (const Vector3f& rkPoint) const
{
    return InSphere(rkPoint,m_kSphere);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SphereBV::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    BoundingVolume::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kSphere.Center);
    rkStream.Read(m_kSphere.Radius);

    WM3_END_DEBUG_STREAM_LOAD(SphereBV);
}
//----------------------------------------------------------------------------
void SphereBV::Link (Stream& rkStream, Stream::Link* pkLink)
{
    BoundingVolume::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SphereBV::Register (Stream& rkStream) const
{
    return BoundingVolume::Register(rkStream);
}
//----------------------------------------------------------------------------
void SphereBV::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    BoundingVolume::Save(rkStream);

    // native data
    rkStream.Write(m_kSphere.Center);
    rkStream.Write(m_kSphere.Radius);

    WM3_END_DEBUG_STREAM_SAVE(SphereBV);
}
//----------------------------------------------------------------------------
StringTree* SphereBV::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(3,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("center =",m_kSphere.Center));
    pkTree->SetString(2,StringTree::Format("radius =",m_kSphere.Radius));

    // children
    pkTree->SetChild(0,BoundingVolume::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int SphereBV::GetMemoryUsed () const
{
    return sizeof(SphereBV) - sizeof(BoundingVolume) +
        BoundingVolume::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int SphereBV::GetDiskUsed () const
{
    return BoundingVolume::GetDiskUsed() +
        sizeof(m_kSphere);
}
//----------------------------------------------------------------------------
