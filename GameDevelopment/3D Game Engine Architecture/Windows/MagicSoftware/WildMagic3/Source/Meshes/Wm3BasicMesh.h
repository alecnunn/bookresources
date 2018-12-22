// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BASICMESH_H
#define WM3BASICMESH_H

#include "Wm3Vector3.h"

namespace Wm3
{

class BasicMesh
{
public:
    // The number of vertices in the input is iVQuantity.  The vertex array
    // is usually passed as akPoint, but this input can be any data type you
    // prefer (points+attributes).  The number of triangles is iTQuantity.
    // The triangles are represented as triples of indices into the vertex
    // array.  These triples are stored in aiIndex.  The caller is responsible
    // for deleting the input arrays.

    BasicMesh (int iVQuantity, const void* akPoint, int iTQuantity,
        const int* aiIndex);

    virtual ~BasicMesh ();

    // The input must correspond to a mesh whose edges are shared by no more
    // than two triangles.  If an edge with three (or more) edges is
    // encountered in the construction, IsValid returns 'false'.  Otherwise
    // it returns 'true'.
    bool IsValid () const;

    class Vertex
    {
    public:
        Vertex ();
        ~Vertex ();

        enum { MV_CHUNK = 8 };

        void InsertEdge (int iV, int iE);
        void InsertTriangle (int iT);

        int VQuantity;
        int* V;
        int* E;
        int TQuantity;
        int* T;
    };

    class Edge
    {
    public:
        Edge ();

        int V[2];
        int T[2];
    };

    class Triangle
    {
    public:
        Triangle ();

        int V[3];
        int E[3];
        int T[3];
    };

    // member access
    int GetVQuantity () const;
    int GetEQuantity () const;
    int GetTQuantity () const;
    const void* GetPoints () const;
    const int* GetIndices () const;
    const Vertex* GetVertices () const;
    const Edge* GetEdges () const;
    const Triangle* GetTriangles () const;

protected:
    int m_iVQuantity, m_iEQuantity, m_iTQuantity;
    const void* m_akPoint;
    const int* m_aiIndex;
    Vertex* m_akVertex;
    Edge* m_akEdge;
    Triangle* m_akTriangle;
    bool m_bIsValid;
};

#include "Wm3BasicMesh.inl"

}

#endif
