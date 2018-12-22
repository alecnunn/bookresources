// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3STANDARDMESH_H
#define WM3STANDARDMESH_H

#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM StandardMesh
{
public:
    StandardMesh ();
    ~StandardMesh ();

    // parameters to control the mesh generation
    StandardMesh& Transform (const Transformation& rkXFrm);
    StandardMesh& Vertices (bool bCached);
    StandardMesh& Indices (bool bCached);
    StandardMesh& Normals (bool bCreate, bool bCached);
    StandardMesh& UVs (bool bCreate, bool bCached);
    StandardMesh& Inside (bool bInside);

    // texture coordinates
    Vector2fArrayPtr GetUVs () const;

    // Standard meshes.  Each mesh is centered at (0,0,0) and has an up-axis
    // of (0,0,1).  The other axes forming the coordinate system are (1,0,0)
    // and (0,1,0).  An application may transform the meshes as necessary.

    TriMesh* Rectangle (int iXSamples, int iYSamples, float fXExtent,
        float fYExtent);

    TriMesh* Disk (int iShellSamples, int iRadialSamples, float fRadius);

    TriMesh* Box (float fXExtent, float fYExtent, float fZExtent);

    TriMesh* Cylinder (int iAxisSamples, int iRadialSamples, float fRadius,
        float fHeight, bool bOpen);

    TriMesh* Sphere (int iZSamples, int iRadialSamples, float fRadius);

    TriMesh* Torus (int iCircleSamples, int iRadialSamples,
        float fOuterRadius, float fInnerRadius);

    // Platonic solids, inscribed in a unit sphere centered at (0,0,0).
    TriMesh* Tetrahedron ();
    TriMesh* Hexahedron ();
    TriMesh* Octahedron ();
    TriMesh* Dodecahedron ();
    TriMesh* Icosahedron ();

private:
    void CreateData (int iVQuantity, int iTQuantity);
    void CreatePlatonicNormals (int iVQuantity);
    void CreatePlatonicUVs (int iVQuantity);
    void ReverseTriangleOrder (int iTQuantity);
    void TransformData (int iVQuantity);

    // temporary storage for mesh creation
    Vector3fArrayPtr m_spkVertices;
    IntArrayPtr m_spkIndices;
    Vector3fArrayPtr m_spkNormals;
    Vector2fArrayPtr m_spkUVs;
    Vector3f* m_akVertex;
    int* m_aiIndex;
    Vector3f* m_akNormal;
    Vector2f* m_akUV;

    Transformation m_kXFrm;

    bool m_bCachedVertices;  // default = false
    bool m_bCachedIndices;   // default = false
    bool m_bCachedNormals;   // default = false
    bool m_bCachedUVs;       // default = false
    bool m_bNormals;         // default = false
    bool m_bUVs;             // default = false
    bool m_bInside;          // default = false
};

#include "Wm3StandardMesh.inl"

}

#endif
