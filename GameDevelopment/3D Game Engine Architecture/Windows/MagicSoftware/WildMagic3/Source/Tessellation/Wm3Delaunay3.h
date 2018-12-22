// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DELAUNAY3_H
#define WM3DELAUNAY3_H

// The Delaunay triangulation method is a modification of code written by 
// Dave Watson.  It uses the incremental algorithm described in
//
//     Watson, D.F., 1981, Computing the n-dimensional Delaunay 
//     tessellation with application to Voronoi polytopes: 
//     The Computer J., 24(2), p. 167-172.

#include "Wm3Vector3.h"
#include "Wm3TriangleKey.h"
#include <map>

namespace Wm3
{

template <class Real>
class WM3_ITEM Delaunay3
{
public:
    // The input to the constructor is the array of vertices whose Delaunay
    // tetrahedralization is required.  If you want Delaunay3 to delete the
    // vertices during destruction, set bOwner to 'true'.  Otherwise, you
    // own the vertices and must delete them yourself.
    //
    // The final four parameters with default values are from Dave Watson's
    // code.  This class needs a major rewrite because it is not robust.  The
    // parameters need to go away.  The code should dynamically allocate
    // whatever it needs.
    Delaunay3 (int iVQuantity, Vector3<Real>* akVertex, bool bOwner,
        Real fEpsilon = (Real)1.0e-5, Real fRange = (Real)10.0,
        int iTSize = (int)75, int iQuantityFactor = (int)16);

    ~Delaunay3 ();

    // access to the tetrahedralization
    int GetVertexQuantity () const;
    const Vector3<Real>* GetVertices () const;
    int GetTetrahedronQuantity () const;
    const int* GetIndices () const;
    const int* GetAdjacencies () const;

    // The quantities are
    //   VQ = GetVertexQuantity() is the input vertex quantity
    //   V  = GetVertices() is the input vertex array
    //   TQ = GetTetrahedronQuantity() is the number of output tetrahedra
    //   I  = GetIndices() is the array of triples of indices into akV that
    //        represent the tetrahedra.  There are 4*TQ of these.
    //   A  = GetAdjacencies() is the array of triples of indices for
    //        tetrahedra.  Each index i satisfies 0 <= i < TQ.  Each triple
    //        represents the tetrahedron adjacent to a given tetrahedron.
    // The i-th tetrahedron in the tetrahedralization has vertices
    //   vertex[0] = V[I[4*i]]
    //   vertex[1] = V[I[4*i+1]]
    //   vertex[2] = V[I[4*i+2]]
    //   vertex[3] = V[I[4*i+3]]
    // The i-th tetrahedron has face index triples listed below.  The face
    // vertex ordering when viewed from outside the tetrahedron is listed.
    //   face[0] = <I[4*i+1],I[4*i+2],I[4*i+3]>, clockwise
    //   face[1] = <I[4*i  ],I[4*i+2],I[4*i+3]>, counterclockwise
    //   face[2] = <I[4*i  ],I[4*i+1],I[4*i+3]>, clockwise
    //   face[3] = <I[4*i  ],I[4*i+1],I[4*i+2]>, counterclockwise
    // The tetrahedra adjacent to these faces have indices
    //   adjacent[0] = A[4*i  ]
    //   adjacent[1] = A[4*i+1]
    //   adjacent[2] = A[4*i+2]
    //   adjacent[3] = A[4*i+3]
    // If there is no adjacent tetrahedron to face[j], the index A[4*i+j]
    // is -1.

    // Locate those tetrahedra faces that do not share other tetrahedra.
    // The returned quantity is the number of triangles in the hull.  The
    // returned array has 3*quantity indices, each triple representing a
    // triangle.  The triangles are counterclockwise ordered when viewed
    // from outside the hull.
    void GetHull (int& riTQuantity, int*& raiIndex) const;

    // Support for searching the tetrahedralization for a tetrahedron that
    // contains a point.  If there is a containing tetrahedron, the returned
    // value is a tetrahedron index i with 0 <= i < riTQuantity.  If there is
    // not a containing tetrahedron, -1 is returned.
    int GetContainingTetrahedron (const Vector3<Real>& rkP) const;

    // Get the vertices for tetrahedron i.  The function returns 'true' if i
    // is a valid tetrahedron index, in which case the vertices are valid.
    // Otherwise, the function returns 'false' and the vertices are invalid.
    bool GetVertices (int i, Vector3<Real>& rkV0, Vector3<Real>& rkV1,
        Vector3<Real>& rkV2, Vector3<Real>& rkV3) const;

    // Get the vertex indices for tetrahedron i.  The function returns 'true'
    // if i is a valid tetrahedron index, in which case the vertices are
    // valid.  Otherwise, the function returns 'false' and the vertices are
    // invalid.
    bool GetIndices (int i, int aiIndex[4]) const;

    // Get the indices for tetrahedra adjacent to tetrahedron i.  The function
    // returns 'true' if i is a valid tetrahedron index, in which case the
    // adjacencies are valid.  Otherwise, the function returns 'false' and
    // the adjacencies are invalid.
    bool GetAdjacencies (int i, int aiAdjacent[4]) const;

    // Compute the barycentric coordinates of P with respect to tetrahedron i.
    // The function returns 'true' if i is a valid tetrahedron index, in which
    // case the coordinates are valid.  Otherwise, the function returns
    // 'false' and the coordinate array is invalid.
    bool GetBarycentrics (int i, const Vector3<Real>& rkP, Real afBary[4])
        const;

private:
    typedef std::pair<int,int> ITuple2;
    typedef std::map<TriangleKey,ITuple2> TMap;
    typedef std::map<TriangleKey,ITuple2>::iterator TMapIterator;

    // Create a mesh of tetrahedra for traversal purposes.  The returned
    // array has 4*GetTetrahedron() elements.  The elements array[4*i+j]
    // for 0 <= j <= 3 correspond to tetrahedron[i].  The tetrahedron
    // tetrahedron[array[4*i+j]] is the one that is adjacent to the triangle
    // face of tetrahedron[i] formed by the m_aiVertex[] values that do *not*
    // include index j.  For example, tetrahedron[array[4*i+2]] is adjacent
    // to the face formed by vertices tetrahedron[i].m_aiVertex[0],
    // tetrahedron[i].m_aiVertex[1], and tetrahedron[i].m_aiVertex[3].  If
    // there is no adjacent tetrahedron to a face, the mesh index is -1.
    void ComputeAdjacencies ();

    // The face vertices are counterclockwise when viewed from outside the
    // tetrahedron.  The point P is outside the tetrahedron when
    // Dot(P-V0,Cross(V1-V0,V2-V0)) > 0.
    static bool PointOutsideFace (const Vector3<Real>& rkP,
        const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
        const Vector3<Real>& rkV2);

    // the tetrahedron mesh
    int m_iVQuantity;
    Vector3<Real>* m_akVertex;
    int m_iTQuantity;
    int* m_aiIndex;
    int* m_aiAdjacent;

    bool m_bOwner;
};

typedef Delaunay3<float> Delaunay3f;
typedef Delaunay3<double> Delaunay3d;

}

#endif
