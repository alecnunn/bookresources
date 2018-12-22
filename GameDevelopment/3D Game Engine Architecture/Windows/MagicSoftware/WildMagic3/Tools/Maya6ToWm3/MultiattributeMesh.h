// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#ifndef MULTIATTRIBUTEMESH_H
#define MULTIATTRIBUTEMESH_H

#include "Vertex.h"
#include "MeshData.h"

#include "Wm3TriMesh.h"
#include "Wm3Vector2Array.h"
#include "Wm3TArray.h"

// Usage:
//
// MultiattributeMesh kMAMesh;
// AddVertexLoc(0,spatial information)
// AddVertexLoc(1,spatial information)
// AddVertexLoc(2,spatial information)
// ...
// int iV0 = kMAMesh.UpdateVertexInfo(0,face specific info);
// int iV1 = kMAMesh.UpdateVertexInfo(1,face specific info);
// int iV2 = kMAMesh.UpdateVertexInfo(2,face specific info);
// kMAMesh.AddTriangle(iV0,iV1,iV2);
//
// Repeat the last four lines for each triangle.

using namespace Wm3;

class MultiattributeMesh
{
public:
    MultiattributeMesh ();
    void Reset ();

    // Adds spatial data into array at this location
    void AddVertexLoc(int iV, const Vertex& rkVertex);

    // Adds UV/normal info, returning vertex index
    // May create "extra" vertices if the vertex has multi-valued
    // information.
    int UpdateVertexInfo(int iV, const Vertex& rkVertex);

    void AddTriangle (int iV0, int iV1, int iV2);
    TriMesh* GetMesh (Vector2fArray*& rpkUV0, Vector2fArray*& rpkUV1,
        Vector2fArray*& rpkUV2, Vector2fArray*& rpkUV3);

    MeshData GetData();

    enum { MAX_TEX_COORDS = 4 };

protected:

    TArray<Vertex> m_kVerts;
    int m_iVNext;
    TArray<int> m_kConnect;

    // Original verts -- ones created by AddVertexLoc, and not
    // ones created as a result of discontinuous information in AddVertexInfo
    int iOriginalVerts;

    // m_kVerts[i] was created from vertex 
    // m_kVerts[m_kVertMapping[i-OriginalVerts]] if i >= iOriginalVerts
    TArray<int> m_kVertMapping;
};

#endif

