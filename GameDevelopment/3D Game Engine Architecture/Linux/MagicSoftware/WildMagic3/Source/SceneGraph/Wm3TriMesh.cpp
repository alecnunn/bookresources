// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3TriMesh.h"
#include "Wm3IntrRay3Triangle3.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,TriMesh,Geometry);
WM3_IMPLEMENT_STREAM(TriMesh);
WM3_IMPLEMENT_DEFAULT_STREAM(TriMesh,Geometry);
WM3_IMPLEMENT_DEFAULT_NAME_ID(TriMesh,Geometry);

//----------------------------------------------------------------------------
TriMesh::TriMesh ()
{
    GeometryType = GT_TRIMESH;
}
//----------------------------------------------------------------------------
TriMesh::TriMesh (Vector3fArrayPtr spkVertices, IntArrayPtr spkIndices,
    bool bGenerateNormals, bool bCachedNormals)
    :
    Geometry(spkVertices)
{
    GeometryType = GT_TRIMESH;
    Indices = spkIndices;
    if ( bGenerateNormals )
        GenerateNormals(bCachedNormals);
}
//----------------------------------------------------------------------------
TriMesh::~TriMesh ()
{
}
//----------------------------------------------------------------------------
void TriMesh::GetModelTriangle (int i, Triangle3f& rkMTri) const
{
    assert( 0 <= i && 3*i < Indices->GetQuantity() );
    Vector3f* akVertex = Vertices->GetData();
    assert( akVertex );
    int* aiIndex = Indices->GetData();
    assert( aiIndex );
    int* piIndex = &aiIndex[3*i];
    rkMTri.V[0] = akVertex[*piIndex++];
    rkMTri.V[1] = akVertex[*piIndex++];
    rkMTri.V[2] = akVertex[*piIndex];
}
//----------------------------------------------------------------------------
void TriMesh::GetWorldTriangle (int i, Triangle3f& rkWTri) const
{
    assert( 0 <= i && 3*i < Indices->GetQuantity() );
    Vector3f* akVertex = Vertices->GetData();
    assert( akVertex );
    int* aiIndex = Indices->GetData();
    assert( aiIndex );
    int* piIndex = &aiIndex[3*i];
    rkWTri.V[0] = World.ApplyForward(akVertex[*piIndex++]);
    rkWTri.V[1] = World.ApplyForward(akVertex[*piIndex++]);
    rkWTri.V[2] = World.ApplyForward(akVertex[*piIndex  ]);
}
//----------------------------------------------------------------------------
void TriMesh::GenerateNormals (bool bCached)
{
    int iVQuantity;
    Vector3f* akNormal;

    if ( !Normals )
    {
        iVQuantity = Vertices->GetQuantity();
        akNormal = new Vector3f[iVQuantity];
        if ( bCached )
            Normals = new CachedVector3fArray(iVQuantity,akNormal);
        else
            Normals = new Vector3fArray(iVQuantity,akNormal);
        UpdateModelNormals();
    }
    else
    {
        if ( bCached && !Normals->IsExactly(CachedVector3fArray::TYPE) )
        {
            iVQuantity = Normals->GetQuantity();
            akNormal = new Vector3f[iVQuantity];
            Normals = new CachedVector3fArray(iVQuantity,akNormal);
            UpdateModelNormals();
        }
        else if ( !bCached && Normals->IsExactly(CachedVector3fArray::TYPE) )
        {
            iVQuantity = Normals->GetQuantity();
            akNormal = new Vector3f[iVQuantity];
            Normals = new Vector3fArray(iVQuantity,akNormal);
            UpdateModelNormals();
        }
    }
}
//----------------------------------------------------------------------------
void TriMesh::UpdateModelNormals ()
{
    if ( !Normals )
        return;

    // Calculate normals from vertices by weighted averages of facet planes
    // that contain the vertices.

    int iVQuantity = Vertices->GetQuantity();
    Vector3f* akVertex = Vertices->GetData();
    Vector3f* akNormal = Normals->GetData();
    memset(Normals->GetData(),0,iVQuantity*sizeof(Vector3f));

    int iTQuantity = Indices->GetQuantity()/3;
    int* piIndex = Indices->GetData();
    int i;
    for (i = 0; i < iTQuantity; i++)
    {
        // get vertex indices
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;

        // get vertices
        Vector3f& rkV0 = akVertex[iV0];
        Vector3f& rkV1 = akVertex[iV1];
        Vector3f& rkV2 = akVertex[iV2];

        // compute the normal (length provides the weighted sum)
        Vector3f kEdge1 = rkV1 - rkV0;
        Vector3f kEdge2 = rkV2 - rkV0;
        Vector3f kNormal = kEdge1.Cross(kEdge2);

        akNormal[iV0] += kNormal;
        akNormal[iV1] += kNormal;
        akNormal[iV2] += kNormal;
    }

    for (i = 0; i < iVQuantity; i++)
        akNormal[i].Normalize();
}
//----------------------------------------------------------------------------
TriMesh::PickRecord::PickRecord (TriMesh* pkIObject, float fT, int iTriangle,
    float fBary0, float fBary1, float fBary2)
    :
    Geometry::PickRecord(pkIObject,fT)
{
    Triangle = iTriangle;
    Bary0 = fBary0;
    Bary1 = fBary1;
    Bary2 = fBary2;
}
//----------------------------------------------------------------------------
void TriMesh::DoPick (const Ray3f& rkRay, PickArray& rkResults)
{
    if ( WorldBound->TestIntersection(rkRay) )
    {
        // convert the ray to model-space coordinates
        Ray3f kRay(World.ApplyInverse(rkRay.Origin),
            World.InvertVector(rkRay.Direction));

        // compute intersections with the model-space triangles
        const Vector3f* akVertex = Vertices->GetData();
        int iTQuantity = Indices->GetQuantity()/3;
        const int* piConnect = Indices->GetData();
        for (int i = 0; i < iTQuantity; i++)
        {
            int iV0 = *piConnect++;
            int iV1 = *piConnect++;
            int iV2 = *piConnect++;

            Triangle3f kTriangle(akVertex[iV0],akVertex[iV1],akVertex[iV2]);
            IntrRay3Triangle3f kIntr(kRay,kTriangle);
            if ( kIntr.Find() )
            {
                rkResults.Append(new PickRecord(this,kIntr.GetRayT(),i,
                    kIntr.GetTriB0(),kIntr.GetTriB1(),kIntr.GetTriB2()));
            }
        }
    }
}
//----------------------------------------------------------------------------
