// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TRIMESH_H
#define WM3TRIMESH_H

#include "Wm3Geometry.h"
#include "Wm3IntArray.h"
#include "Wm3Triangle3.h"

namespace Wm3
{

class WM3_ITEM TriMesh : public Geometry
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    TriMesh (Vector3fArrayPtr spkVertices, IntArrayPtr spkIndices,
        bool bGenerateNormals, bool bCachedNormals = false);

    virtual ~TriMesh ();

    // member access
    int GetTriangleQuantity () const;
    void GetModelTriangle (int i, Triangle3f& rkMTri) const;
    void GetWorldTriangle (int i, Triangle3f& rkWTri) const;

    void GenerateNormals (bool bCached = false);

    // Picking support.  The pick record stores the index of any triangle
    // intersected by the ray.  It stores the barycentric coordinates of
    // the intersection point.  This allows the application to compute
    // interpolated vertex attributes as well as the actual point of
    // intersection.
    class WM3_ITEM PickRecord : public Geometry::PickRecord
    {
    public:
        PickRecord (TriMesh* pkIObject, float fT, int iTriangle, float fBary0,
            float fBary1, float fBary2);

        // Index of the triangle that is intersected by the ray.
        int Triangle;

        // Barycentric coordinates of the point of intersection.  If b0, b1,
        // and b2 are the values, then all are in [0,1] and b0+b1+b2=1.
        float Bary0, Bary1, Bary2;
    };

    // The origin and direction of the ray must be in world coordinates.  The
    // application is responsible for deleting the pick records in the array.
    virtual void DoPick (const Ray3f& rkRay, PickArray& rkResults);

protected:
    TriMesh ();

    // geometric updates
    virtual void UpdateModelNormals ();
};

WM3_REGISTER_STREAM(TriMesh);
typedef Pointer<TriMesh> TriMeshPtr;
#include "Wm3TriMesh.inl"

}

#endif
