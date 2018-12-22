// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DELAUNAY2_H
#define WM3DELAUNAY2_H

#include "Wm3Vector2.h"
#include <map>
#include <set>
#include <vector>

namespace Wm3
{

template <class Real>
class WM3_ITEM Delaunay2
{
public:
    // The input to the constructor is the array of vertices whose Delaunay
    // triangulation is required.  If you want Delaunay2 to delete the
    // vertices during destruction, set bOwner to 'true'.  Otherwise, you
    // own the vertices and must delete them yourself.
    Delaunay2 (int iVQuantity, Vector2<Real>* akVertex, bool bOwner);
    ~Delaunay2 ();

    // access to the triangulation
    int GetVertexQuantity () const;
    const Vector2<Real>* GetVertices () const;
    int GetTriangleQuantity () const;
    const int* GetIndices () const;
    const int* GetAdjacencies () const;

    // The quantities are
    //   VQ = GetVertexQuantity() is the input vertex quantity
    //   V  = GetVertices() is the input vertex array
    //   TQ = GetTriangleQuantity() is the number of output triangles
    //   I  = GetIndices() is the array of triples of indices into akV that
    //        represent the triangles.  There are 3*TQ of these.
    //   A  = GetAdjacencies() is the array of triples of indices for
    //        triangles.  Each index i satisfies 0 <= i < TQ.  Each triple
    //        represents the triangle adjacent to a given triangle.
    // The i-th triangle in the triangulation has vertices
    //   vertex[0] = V[I[3*i]]
    //   vertex[1] = V[I[3*i+1]]
    //   vertex[2] = V[I[3*i+2]]
    // The i-th triangle has edge index pairs
    //   edge[0] = <I[3*i],I[3*i+1]>
    //   edge[1] = <I[3*i+1],I[3*i+2]>
    //   edge[2] = <I[3*i+2],I[3*i]>
    // The triangles adjacent to these edges have indices
    //   adjacent[0] = A[3*i]
    //   adjacent[1] = A[3*i+1]
    //   adjacent[2] = A[3*i+2]
    // If there is no adjacent triangle to edge[j], the index A[3*i+j] is -1.

    // Locate those triangle edges that do not share other triangles.  The
    // returned quantity is the number of edges in the hull.  The returned
    // array has 2*quantity indices, each pair representing an edge.  The
    // edges are not ordered, but the pair of vertices for an edge is ordered
    // so that they conform to a counterclockwise traversal of the hull.
    void GetHull (int& riEQuantity, int*& raiIndex);

    // Support for searching the triangulation for a triangle that contains
    // a point.  If there is a containing triangle, the returned value is a
    // triangle index i with 0 <= i < riTQuantity.  If there is not a
    // containing triangle, -1 is returned.
    int GetContainingTriangle (const Vector2<Real>& rkP) const;

    // Get the vertices for triangle i.  The function returns 'true' if i is
    // a valid triangle index, in which case the vertices are valid.
    // Otherwise, the function returns 'false' and the vertices are invalid.
    bool GetVertices (int i, Vector2<Real>& rkV0, Vector2<Real>& rkV1,
        Vector2<Real>& rkV2) const;

    // Get the vertex indices for triangle i.  The function returns 'true' if
    // i is a valid triangle index, in which case the vertices are valid.
    // Otherwise, the function returns 'false' and the vertices are invalid.
    bool GetIndices (int i, int aiIndex[3]) const;

    // Get the indices for triangles adjacent to triangle i.  The function
    // returns 'true' if i is a valid triangle index, in which case the
    // adjacencies are valid.  Otherwise, the function returns 'false' and
    // the adjacencies are invalid.
    bool GetAdjacencies (int i, int aiAdjacent[3]) const;

    // Compute the barycentric coordinates of P with respect to triangle i.
    // The function returns 'true' if i is a valid triangle index, in which
    // case the coordinates are valid.  Otherwise, the function returns
    // 'false' and the coordinate array is invalid.
    bool GetBarycentrics (int i, const Vector2<Real>& rkP, Real afBary[3])
        const;

private:
    // for sorting to remove duplicate input points
    class WM3_ITEM Vertex
    {
    public:
        Vertex () { /**/ }

        Vertex (const Vector2<Real>& rkV, int iIndex)
            :
            V(rkV)
        {
            Index = iIndex;
        }

        bool operator== (const Vertex& rkSV) const
        {
            return V == rkSV.V;
        }

        bool operator!= (const Vertex& rkSV) const
        {
            return !(V == rkSV.V);
        }

        bool operator<  (const Vertex& rkSV) const
        {
            if ( V.X() < rkSV.V.X() )
                return true;
            if ( V.X() > rkSV.V.X() )
                return false;
            return V.Y() < rkSV.V.Y();
        }

        Vector2<Real> V;
        int Index;
    };
    typedef typename std::vector<Vertex> VArray;

    // triangles
    class WM3_ITEM Triangle
    {
    public:
        Triangle ()
        {
            for (int i = 0; i < 3; i++)
            {
                V[i] = -1;
                A[i] = 0;
            }
        }

        Triangle (int iV0, int iV1, int iV2, Triangle* pkA0, Triangle* pkA1,
            Triangle* pkA2)
        {
            V[0] = iV0;
            V[1] = iV1;
            V[2] = iV2;
            A[0] = pkA0;
            A[1] = pkA1;
            A[2] = pkA2;
        }

        // vertices, listed in counterclockwise order
        int V[3];

        // adjacent triangles,
        //   A[0] points to triangle sharing edge (V[0],V[1])
        //   A[1] points to triangle sharing edge (V[1],V[2])
        //   A[2] points to triangle sharing edge (V[2],V[0])
        Triangle* A[3];
    };

    typedef typename std::set<Triangle*> TSet;
    typedef typename TSet::iterator TSetIterator;
    typedef typename std::vector<Triangle*> TArray;

    // edges (to support constructing the insertion polygon)
    class WM3_ITEM Edge
    {
    public:
        Edge (int iV0 = -1, int iV1 = -1, Triangle* pkT = 0,
            Triangle* pkA = 0)
        {
            V0 = iV0;
            V1 = iV1;
            T = pkT;
            A = pkA;
        }

        int V0, V1;    // ordered vertices
        Triangle* T;   // insertion polygon triangle
        Triangle* A;   // triangle adjacent to insertion polygon
    };

    typedef typename std::map<int,Edge> EMap;  // <V0,(V0,V1,T,A)>
    typedef typename std::vector<Edge> EArray;  // (V0,V1,T,A)

    Triangle* GetContaining (const Vector2<Real>& rkP) const;
    bool IsInsertionComponent (const Vector2<Real>& rkV, Triangle* pkTri)
        const;
    void GetInsertionPolygon (const Vector2<Real>& rkV, TSet& rkPolyTri)
        const;
    void GetInsertionPolygonEdges (TSet& rkPolyTri, EArray& rkPoly) const;
    void AddTriangles (int iV2, const EArray& rkPoly);
    void RemoveInsertionPolygon (TSet& rkPolyTri);
    void RemoveTriangles ();

    static bool PointInCircle (const Vector2<Real>& rkP,
        const Vector2<Real>& rkV0, const Vector2<Real>& rkV1,
        const Vector2<Real>& rkV2);

    static bool PointOutsideEdge (const Vector2<Real>& rkP,
        const Vector2<Real>& rkV0, const Vector2<Real>& rkV1);

    static bool PointInTriangle (const Vector2<Real>& rkP,
        const Vector2<Real>& rkV0, const Vector2<Real>& rkV1,
        const Vector2<Real>& rkV2);

    // sorted input vertices for processing
    VArray m_kVertex;

    // indices for the supertriangle vertices
    int m_aiSuperV[3];

    // triangles that contain a supertriangle edge
    Triangle* m_apkSuperT[3];

    // the current triangulation
    TSet m_kTriangle;

    // the triangle mesh
    int m_iVQuantity;
    Vector2<Real>* m_akVertex;
    int m_iTQuantity;
    int* m_aiIndex;
    int* m_aiAdjacent;

    bool m_bOwner;
};

typedef Delaunay2<float> Delaunay2f;
typedef Delaunay2<double> Delaunay2d;

}

#endif
