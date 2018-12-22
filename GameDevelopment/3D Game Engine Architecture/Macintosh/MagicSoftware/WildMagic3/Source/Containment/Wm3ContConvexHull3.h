// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTCONVEXHULL3_H
#define WM3CONTCONVEXHULL3_H

#include "Wm3Vector3.h"
#include "Wm3ETManifoldMesh.h"
#include <map>
#include <set>
#include <stack>
#include <vector>

namespace Wm3
{

template <class Real>
class WM3_ITEM ConvexHull3
{
public:
    // Construction and destruction.  ConvexHull3 does not take ownership
    // of the input array.  The application is responsible for deleting it.
    ConvexHull3 (int iVQuantity, const Vector3<Real>* akVertex);
    ~ConvexHull3 ();

    // hull types
    enum
    {
        // Hull is a single point.  Quantity is 1, index array has one
        // element (index 0).
        HULL_POINT,

        // Hull is a line segment.  Quantity is 2, index array has two
        // elements that are indices to the end points of the segment.
        HULL_LINEAR,

        // Hull is a planar convex polygon.  Quantity is number of vertices,
        // index array has the indices to that number of vertices.  The
        // indices represent an ordered polygon, but since there is no
        // associated normal vector, you need to supply your own and determine
        // if the ordering is clockwise or counterclockwise relative to that
        // normal.  If you want a triangle connectivity array, you will have
        // to triangulate the polygon yourself.
        HULL_PLANAR,

        // The hull is a convex polyhedron (positive volume).  The number of
        // triangles is GetIndices().size()/3 and each triple of indices
        // represents a triangle in the hull.  All the triangles are
        // counterclockwise ordered as you view the polyhedron from the
        // outside.
        HULL_SPATIAL
    };

    int GetType () const;
    const std::vector<int>& GetIndices () const;

    // These are valid only for HULL_PLANAR convex hulls.
    const Vector3<Real>& GetPlaneOrigin () const;
    const Vector3<Real>& GetPlaneNormal () const;

private:
    // construct convex hull incrementally
    void MergePoint (int iP);
    void MergeLinear (int iP);
    void MergePlanar (int iP);
    void MergeSpatial (int iP);

    class WM3_ITEM Triangle : public ETManifoldMesh::Triangle
    {
    public:
        Triangle (int iV0, int iV1, int iV2)
            :
            ETManifoldMesh::Triangle(iV0,iV1,iV2)
        {
        }

        Vector3<Real> Normal;  // outward normal to triangle
        Real Order;
        bool Visited;
    };

    static ETManifoldMesh::Triangle* CreateTriangle (int iV0, int iV1,
        int iV2);
    void InsertTriangle (int iV0, int iV1, int iV2);

    // for terminator construction
    class WM3_ITEM Edge
    {
    public:
        Edge (int iV0, int iV1, Triangle* pkAdj)
        {
            Set(iV0,iV1,pkAdj);
            Prev = 0;
            Next = 0;
        }

        void Set (int iV0, int iV1, Triangle* pkAdj)
        {
            V0 = iV0;
            V1 = iV1;
            Adj = pkAdj;
            for (int i = 0; i < 3; i++)
            {
                V2 = Adj->V[i];
                if ( V2 != V0 && V2 != V1 )
                    break;
            }
        }

        Triangle* GetAdj (int iV0) const
        {
            Triangle* pkAdj = 0;
            for (int i = 0; i < 3; i++)
            {
                if ( Adj->V[i] == iV0 )
                {
                    pkAdj = (Triangle*)Adj->T[i];
                    break;
                }
            }
            assert( pkAdj );
            return pkAdj;
        }

        int V0, V1, V2;
        Triangle* Adj;

        Edge* Prev;
        Edge* Next;
    };

    Edge* GetPolygon (const std::set<Triangle*>& rkInterior) const;
    void FillHole (std::set<Triangle*>& rkInterior, Triangle* pkMinTri);
    void DeleteEdgeList (Edge* pkList);

    // hull information
    int m_iHullType;
    std::vector<int> m_kHull;

    // input vertices (scaled to [-1,1]^3)
    int m_iVQuantity;
    Vector3<Real>* m_akVertex;

    // point, linear, or planar hull
    std::vector<int> m_kHullP;
    Vector3<Real> m_kOrigin, m_kNormal;

    // spatial hull
    ETManifoldMesh m_kHullS;
};

typedef ConvexHull3<float> ConvexHull3f;
typedef ConvexHull3<double> ConvexHull3d;

}

#endif
